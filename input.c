#include <SDL2/SDL.h>
#include "input.h"
#include "internal.h"

static Input input = {
    .keys = {0},
};

void input_event(SDL_Event e) {
    switch(e.type) {
    case SDL_KEYDOWN:
        input.keys[e.key.keysym.scancode] = 1;
        break;
    case SDL_KEYUP:
        input.keys[e.key.keysym.scancode] = 0;
        break;
    }
}

Input input_update()
{
    Input input_copy = input;
    for(int i=0; i<SDL_NUM_SCANCODES; ++i) {
        if (input.keys[i]) input.keys[i]++;
    }
    return input_copy;
}

