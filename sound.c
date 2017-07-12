#include <SDL2/SDL.h>
#include <math.h>

#include "sound.h"

#define MEASURE 32

static int srate,   // sample rate
    samp_count = 0, // samples passed; used for ticking
    tick_next  = 0, // at what sample count is next tick
    tick_count = 0, // ticks elapsed; used for music
    tick_len,       // samples until next tick
    phase      = 0, // current wave phase (never is reset lol)
    phase_inc  = 0, // amount to increase phase to get desired pitch
    mode,           // type of sound to play
    interrupt  = 0, // countdown to stop playing music
    level      = 0, // gameplay level; sort of a track number
    mute = 0;

static double sweep;

enum mode {
    MODE_NOISE,
    MODE_BEEP,
};

static const char noise[] = "u$a71i0Rkk*1LkQ46d2Dqtau4Pn1cU;tZ8G'#Xsn_};-&)<)n{z!^r5J|FISa@/6#P9uEDq^_ e$v=.#T*$48S_6y*jvd5.$fbyGBl(=))B;`PX7tve/K`E`'KYka#+c!AFCIhalq?*no|'!ul0BOp%pWxwT7%99FzvodYxX4$b?iA3qQV]uN7+HkBMA[/=n:#I4u^RY}o^Z$RHtJ''yX2]`UF#LUZ<+wZozHd5S_pXM:hw[p.6>kvs35$oxJh6&";

void sound_toggle()
{
    mute = !mute;
}

void sound_level(int new_level)
{
    level = new_level;
}

void nosound()
{
    phase_inc = 0;
}

void beep_sweep(int pitch, double new_sweep)
{
    phase_inc = srate / pow(2.0, (pitch-60)/12.0);
    mode = MODE_BEEP;
    sweep = new_sweep;
    interrupt = 2;
}

void beep(int pitch)
{
    beep_sweep(pitch, 0);
}

static void drum(int depth, int new_sweep)
{
    mode = MODE_NOISE;
    phase_inc = depth;
    sweep = new_sweep;
}

static void next_measure()
{
}

static void tick()
{
    phase_inc -= phase_inc * sweep;
    tick_next = samp_count + tick_len;

    if(!interrupt) {
        switch(tick_count % MEASURE) {
        case 0: drum(10, 0.5); break;
        case 1: break;
        case 8: if(level) drum(400, 0); break;
        case 20: drum(10, 0.5); break;
        case 21: break;
        case 24: if(level) drum(400, 0); break;
        case MEASURE-1: next_measure(); // fallthrough
        default: nosound();// play melody/bass
        }
    } else {
        --interrupt;
    }

    tick_count++;
}

static void callback(void* data, Uint8* stream, int len)
{
    Sint16* stream16 = (Sint16*)stream;
    if(mute) {
        memset(stream16, 0, len);
        return;
    }

    for(int i=0; i<len/2;) {
        for(int c=0; c<2; ++c) {
            if(samp_count == tick_next) tick();
            samp_count++;
            if(mode == MODE_NOISE) {
                Uint8 off_phase = (Uint8)(phase/0xff) + c*64;
                stream16[i] = noise[off_phase] * 0xff - 0x7f;
            } else {
                Sint8 off_phase = (Sint8)(phase/0xffff) + c*64;
                stream16[i] = (off_phase < 0) * 0x8000 - 0x4000;
            }
            i++;
        }
        phase += phase_inc;
    }
}

void sound_init()
{
    SDL_AudioSpec want = {
        .freq = 48000,
        .format = AUDIO_S16,
        .channels = 2,
        .callback = callback,
    };
    SDL_AudioSpec have = {0};
    int dev = SDL_OpenAudioDevice(
        NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    srate = have.freq;
    tick_len = srate / MEASURE * 2;
    SDL_PauseAudioDevice(dev, 0);
}
