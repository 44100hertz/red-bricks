// #include <SDL2/SDL.h>

#define GAME_W 240
#define GAME_H 160

// #include <SDL2/SDL.h>
// #include "Input.h"

typedef SDL_Window* Win;
typedef SDL_Renderer* Rdr;

typedef struct {
    void (*new)(void* data, Rdr rdr);
    void (*draw)(void* data, Rdr rdr, double thru);
    int (*update)(Input input, void* data);
    void (*free)(void* data);
    void* data;
} Scene;

typedef struct {
    double x, y;
} Point;

static SDL_Point point_to_sdl(Point p) {
    return (SDL_Point){(int)p.x, (int)p.y};
}

typedef struct {
    Point pos;
    Point vel;
} Moving;

static Point moving_moved(Moving m) {
    return (Point){m.pos.x + m.vel.x, m.pos.y + m.vel.y};
}
static Point moving_lerp(Moving m, double thru) {
    return (Point){m.pos.x + m.vel.x * thru, m.pos.y + m.vel.y * thru};
}

void run_scene(Scene scene);
