#include <SDL2/SDL.h>
#include <math.h>

#include "internal.h"
#include "sound.h"
#include "breakout.h"

typedef struct {
    SDL_Texture* bg;
    int time;
} Menu;

static void draw(void* data, Rdr rdr, double thru)
{
    Menu* menu = data;
    int xoff = sin((menu->time + thru) / 128.0 * 3.141592) * 16;
    int yoff = cos((menu->time + thru) / 128.0 * 3.141592) * 16;
    SDL_Rect draw_to = {xoff, yoff, GAME_W, GAME_H};
    SDL_RenderCopy(rdr, menu->bg, NULL, &draw_to);
}
static int update(Input input, void* data)
{
    Menu* menu = data;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if(keys[SDL_SCANCODE_SPACE]) {
        run_scene(breakout_new(0));
        flash(40);
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
    SDL_Surface* s = SDL_LoadBMP("redbricks.bmp");
    SDL_SetColorKey(s, SDL_TRUE, 0);
    menu->bg = SDL_CreateTextureFromSurface(rdr, s);
}

Scene menu_new()
{
    Scene s = {
        .new = new,
        .draw = draw,
        .update = update,
        .data = calloc(1, sizeof(Menu)),
    };
    return s;
}
