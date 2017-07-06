#include <SDL2/SDL.h>
#include <math.h>
#include "sound.h"

static int srate;
static SDL_AudioDeviceID dev;
static int duty_time;
static int ticks;
static int beep_count = 0;
static int beep_pitch;
static double sweep;

void beep(double note, double len) {
    beep_sweep(note, len, 0);
}

void beep_sweep(double note, double len, double new_sweep) {
    beep_count = len * srate;
    double pitch = 440 * pow(2.0, (note-60)/12.0);
    beep_pitch = srate / pitch;
    sweep = new_sweep;
}

static void callback(void* userdata, Uint8* stream, int len)
{
    static long count = 0;
    Sint16* stream16 = (Sint16*)stream;
    memset(stream16, 0, len);
    for(int i=0; i<len/2; ++i) {
        if(beep_count) {
            stream16[i] = count % beep_pitch < duty_time ? -0x7fff : 0x7fff;
            beep_count--;
        }
        if(sweep && count%ticks == 0) {
            beep_pitch += beep_pitch * sweep;
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
    duty_time = srate / 1000;
    ticks = srate / 12;

    SDL_PauseAudioDevice(dev, 0);
}
