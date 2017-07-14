#include <SDL2/SDL.h>
#include "input.h"
#include "internal.h"
#include "sound.h"

static Win win = NULL;
static Rdr rdr;

static int pause = 0;
static int tick_count = 1;
static int draw_time = 16;
static int start_time = 0;

const static double tick_len = 1000/60.0;

static int hotkey(SDL_Event e)
{
    switch(e.type) {
    case SDL_KEYDOWN:
        switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_M:
            sound_toggle();
            return 1;
        case SDL_SCANCODE_P:
            pause = !pause;
            return 1;
        }
    }
    return 0;
}

void pause_toggle() { pause = !pause; }

void run_scene(Scene scene)
{
    int is_root = !win;
    if(is_root) {
        SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        win = SDL_CreateWindow("hi", -1, -1, GAME_W*3, GAME_H*3,
                               SDL_WINDOW_OPENGL |
                               SDL_WINDOW_SHOWN |
                               SDL_WINDOW_RESIZABLE);
        rdr = SDL_CreateRenderer(win, 0, SDL_RENDERER_PRESENTVSYNC);
        SDL_RenderSetLogicalSize(rdr, GAME_W, GAME_H);

        sound_init();
    }

    if(scene.new) scene.new(scene.data, rdr);

    SDL_Event e;
    while (1) {
        while (SDL_PollEvent(&e)) {
            if(hotkey(e)) continue;
            switch(e.type) {
            case SDL_QUIT: goto quit;
            default: input_event(e);
            }
        }
        if(!pause && !scene.update(input_update(), scene.data))
            goto quit;

        if(!start_time) start_time = SDL_GetTicks();
        int next_tick = start_time + tick_count*tick_len;
        int last_draw;
        while((last_draw = SDL_GetTicks()) < next_tick) {
            double thru = 1.0 + (last_draw - next_tick)/tick_len;

            SDL_SetRenderDrawColor(rdr, 20, 30, 40, 255);
            SDL_RenderClear(rdr);
            SDL_SetRenderDrawColor(rdr, 0, 0, 0, 255);
            const SDL_Rect borders = {0,0,GAME_W,GAME_H};
            SDL_RenderFillRect(rdr, &borders);
            scene.draw(scene.data, rdr, thru);

            SDL_RenderPresent(rdr);
        }
        tick_count++;
    }
quit:

    if(is_root) {
        SDL_DestroyWindow(win);
        SDL_DestroyRenderer(rdr);
        SDL_Quit();
    }

    if(scene.free) scene.free(scene.data);
}
