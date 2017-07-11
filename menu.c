#include <SDL2/SDL.h>
#include <math.h>

#include "internal.h"
#include "sound.h"
#include "breakout.h"

typedef struct {
    SDL_Texture* bg;
    int time;
} Menu;

static void draw(void* data, Rdr rdr)
{
    Menu* menu = data;
    int xoff = sin(menu->time % 256 / 128.0 * 3.141592) * 16;
    int yoff = cos(menu->time % 256 / 128.0 * 3.141592) * 16;
    SDL_Rect draw_to = {xoff, yoff, GAME_W, GAME_H};
    SDL_RenderCopy(rdr, menu->bg, NULL, &draw_to);
}
static int update(void* data)
{
    Menu* menu = data;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if(keys[SDL_SCANCODE_SPACE]) {
        run_scene(breakout_new(0));
        sound_level(0);
    } else if (keys[SDL_SCANCODE_Q]) {
        return 0;
    }
    menu->time++;

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
