#include <SDL2/SDL.h>
#include <math.h>

#include "sound.h"

#define MEASURE 32

typedef struct {
    char* pcm;
    size_t   pcm_len;   // used for looping
    unsigned pcm_speed; // used to calculate phase_inc

    unsigned phase, // current wave phase
        phase_inc,  // phase increase per sample for desired note
        env;        // how long to play sound; 0 = silent

    double sweep;
} Wave;

static int srate,   // sample rate
    samp_count = 0, // samples passed; used for ticking
    tick_next  = 0, // at what sample count is next tick
    tick_count = 0, // ticks elapsed; used for music
    tick_len,       // samples between each tick
    level      = 0, // gameplay level; sort of a track number
    mute = 0;

static char square_data[] = "        aeimptx|";
static char noise_data[] = "u$a71i0Rkk*1LkQ46d2Dqtau4Pn1cU;tZ8G'#Xsn_};-&)<)n{z!^r5J|FISa@/6#P9uEDq^_ e$v=.#T*$48S_6y*jvd5.$fbyGBl(=))B;`PX7tve/K`E`'KYka#+c!AFCIhalq?*no|'!ul0BOp%pWxwT7%99FzvodYxX4$b?iA3qQV]uN7+HkBMA[/=n:#I4u^RY}o^Z$RHtJ''yX2]`UF#LUZ<+wZozHd5S_pXM:hw[p.6>kvs35$oxJh6&";
static Wave square = {
    .pcm = square_data,
    .pcm_len = sizeof(square_data) - 1,
    .pcm_speed = 0x200000,
};
static Wave noise = {
    .pcm = noise_data,
    .pcm_len = sizeof(noise_data) - 1,
    .pcm_speed = 0x1000,
};
static Wave* waves[2] = {
    &square,
    &noise,
};
static const int num_waves = sizeof(waves);

void sound_toggle() { mute = !mute; }
void sound_level(int new_level) { level = new_level; }

void beep(int pitch) { beep_sweep(pitch, 0); }
void beep_sweep(int pitch, double new_sweep)
{
    square.env = new_sweep ? 4 : 2;
    square.phase_inc = srate / pow(2.0, (pitch-60)/12.0);
    square.sweep = new_sweep;
}

static void drum(int depth, int new_sweep)
{
    noise.env = 2;
    noise.phase_inc = depth;
    noise.sweep = new_sweep;
}

static void tick()
{
    tick_next = samp_count + tick_len;

    for(int iw=0; iw<2; ++iw) {
        Wave* w = waves[iw];
        if(w->env) {
            w->env--;
            w->phase_inc *= 1.0 - w->sweep;
        } else {
            w->phase_inc = 0;
        }
    }

    switch(tick_count % MEASURE) {
    case 0: if(level) drum(10, 0.5); break;
    case 8: drum(400, 0); break;
    case 20: if(level) drum(10, 0.5); break;
    case 24: drum(400, 0); break;
    }

    tick_count++;
}

static void callback(void* data, Uint8* stream, int len)
{
    memset(stream, 0, len);
    if(mute) return;

    Sint16* stream16 = (Sint16*)stream;
    for(int i=0; i<len/2; i+=2) {
        if(samp_count == tick_next) tick();
        ++samp_count;
        for(int iw=0; iw<2; ++iw) {
            Wave* w = waves[iw];
            for(int c=0; c<2; ++c) {
                int phase = (w->phase / w->pcm_speed + c*w->pcm_len/4)
                    % w->pcm_len;
                stream16[i+c] += w->pcm[phase] * 0x40;
            }
            w->phase += w->phase_inc;
        }
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
