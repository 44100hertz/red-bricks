#include <SDL2/SDL.h>
#include <math.h>
#include "sound.h"

static int srate;
static SDL_AudioDeviceID dev;
static int ticks;
static int beep_count;
static int beep_len;
static int beep_pitch[2];
static double sweep;

void beep(double note, double len) {
    beep_sweep(note, len, 0);
}

void beep_sweep(double note, double len, double new_sweep) {
    beep_count = len * srate * 2;
    beep_len = beep_count;
    double pitch = 440 * pow(2.0, (note-60)/12.0);
    beep_pitch[0] = srate / pitch;
    beep_pitch[1] = srate / pitch * (255.0/256);
    sweep = new_sweep;
}

static void callback(void* userdata, Uint8* stream, int len)
{
    static long count = 0;
    Sint16* stream16 = (Sint16*)stream;
    memset(stream16, 0, len);
    for(int i=0; i<len/2;) {
        for(int c=0; c<2; ++c) {
            if(beep_count) {
                stream16[i] = count % beep_pitch[c] * 0xffff / beep_pitch[c];
                beep_count--;
            }
            if(sweep && count%ticks == 0) {
                beep_pitch[c] += beep_pitch[c] * sweep;
            }
            i++;
        }
        count++;
    }
}

void audio_init()
{
    SDL_AudioSpec want = {
        .freq = 48000,
        .format = AUDIO_S16,
        .channels = 2,
        .callback = callback,
    };
    SDL_AudioSpec have = {0};
    dev = SDL_OpenAudioDevice(
        NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    srate = have.freq;
    ticks = srate / 12;

    SDL_PauseAudioDevice(dev, 0);
}
