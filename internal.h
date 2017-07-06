// #include <SDL2/SDL.h>

#define GAME_W 240
#define GAME_H 160

typedef SDL_Window* Win;
typedef SDL_Renderer* Rdr;

typedef struct {
    void (*draw)(void* data, Rdr rdr);
    void (*update)(void* data);
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

void run_scene(Scene scene);
