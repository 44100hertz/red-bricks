#include <SDL2/SDL.h>
#include "internal.h"
#include "sound.h"

void run_scene(Scene scene)
{
    static Win win = NULL;
    static Rdr rdr;

    int is_root = !win;
    if(is_root) {
        SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        win = SDL_CreateWindow("hi", -1, -1, GAME_W*3, GAME_H*3, 0);
        rdr = SDL_CreateRenderer(win, 0, SDL_RENDERER_PRESENTVSYNC);
        SDL_RenderSetLogicalSize(rdr, GAME_W, GAME_H);
        audio_init();
    }

    SDL_Event e;
    while (1) {
        while (SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT: goto quit;
            }
        }
        SDL_SetRenderDrawColor(rdr, 0, 0, 0, 255);
        SDL_RenderClear(rdr);

        scene.update(scene.data);
        scene.draw(scene.data, rdr);

        SDL_RenderPresent(rdr);
    }
quit:

    if(is_root) {
        SDL_DestroyWindow(win);
        SDL_DestroyRenderer(rdr);
        SDL_Quit();
    }
}
