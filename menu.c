#include <SDL2/SDL.h>
#include "internal.h"
#include "breakout.h"

typedef struct {
    SDL_Texture* bg;
} Menu;

static void draw(void* data, Rdr rdr)
{
    Menu* menu = data;
    SDL_RenderCopy(rdr, menu->bg, NULL, NULL);
}
static int update(void* data)
{
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if(keys[SDL_SCANCODE_SPACE]) {
        run_scene(breakout_new());
    } else if (keys[SDL_SCANCODE_Q]) {
        return 0;
    }

    return 1;
}

static void new(void* data, Rdr rdr)
{
    Menu* menu = data;
    menu->bg = SDL_CreateTextureFromSurface(
        rdr, SDL_LoadBMP("redbricks.bmp"));
}

Scene menu_new()
{
    Scene s = {
        .new = new,
        .draw = draw,
        .update = update,
        .data = malloc(sizeof(Menu)),
    };
    return s;
}
