#include <SDL2/SDL.h>
#include "internal.h"
#include "breakout.h"
#include "sound.h"

#define BRICKS_X 10
#define PAD 2
#define SPACE_X (GAME_W / BRICKS_X)
#define SPACE_Y (GAME_H / 12)

typedef struct {
    char* bricks;
    int level;
    int num_bricks;
    int bricks_left;
    Moving paddle;
    SDL_Point paddle_size;
    Moving ball;
    int stuck;
} Game;

static SDL_Rect ball_rect(Game* g) {
    const int ball_r = 2;
    SDL_Rect rect = {
        (int)g->ball.pos.x - ball_r,
        (int)g->ball.pos.y - ball_r,
        ball_r*2, ball_r*2,
    };
    return rect;
}
static SDL_Rect paddle_rect(Game* g, Point* paddle) {
    if(!paddle) paddle = &g->paddle.pos;
    SDL_Rect rect = {
        (int)paddle->x - g->paddle_size.x,
        (int)paddle->y,
        g->paddle_size.x*2,
        g->paddle_size.y,
    };
    return rect;
}
static int brick_pos(Point pos) {
    return (int)pos.x/SPACE_X + ((int)pos.y/SPACE_Y)*BRICKS_X;
}
static int brick_on(Game* g, Point pos) {
    int brick = brick_pos(pos);
    if(brick < 0 || brick >= g->num_bricks) return 0;
    return g->bricks[brick];
}

static int update(void* data)
{
    Game* g = data;
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    g->ball.vel.y += (1.0/32);

    if (keys[SDL_SCANCODE_LEFT]) g->paddle.vel.x -= 1;
    if (keys[SDL_SCANCODE_RIGHT]) g->paddle.vel.x += 1;
    if(g->paddle.pos.x < 0 || g->paddle.pos.x > GAME_W) {
        beep(24);
        g->paddle.vel.x *= -3;
    } else {
        g->paddle.vel.x *= (6.0/8);
    }

    if(g->ball.pos.y > GAME_H) {
        beep_sweep(36, 0.1);
        return 0;
    }

    if(g->stuck) {
        if (keys[SDL_SCANCODE_SPACE]) {
            beep_sweep(36, -0.5);
            g->stuck = 0;
            g->ball.vel = (Point){
                .x = 1.0/8 - rand()%2/4.0 + g->paddle.vel.x/2.0,
                .y = -3
            };
        }
        g->paddle.pos = moving_moved(g->paddle);
        g->ball.pos = g->paddle.pos;
        g->ball.pos.y -= 4;
        return 1;
    }

    Point moved_ball = moving_moved(g->ball);
    Point moved_paddle = moving_moved(g->paddle);

    if(moved_ball.x <= 0 || moved_ball.x >= GAME_W) {
        beep(24);
        g->ball.vel.x *= -1;
        moved_ball = moving_moved(g->ball);
    }

    int brick = brick_pos(moved_ball);
    int brick_state = brick_on(g, moved_ball);
    if(brick_state) {
        beep(48);
        Point moved_x = {g->ball.pos.x, moved_ball.y};
        Point moved_y = {moved_ball.x, g->ball.pos.y};
        if(!brick_on(g, moved_x)) g->ball.vel.x *= -1;
        if(!brick_on(g, moved_y)) g->ball.vel.y *= -1;
        moved_ball = moving_moved(g->ball);
        g->bricks[brick] = 0;
        g->bricks_left--;
    } else {
        SDL_Rect paddle = paddle_rect(g, &moved_paddle);
        paddle.h = 1;
        SDL_Point p1 = point_to_sdl(g->ball.pos);
        SDL_Point p2 = point_to_sdl(moved_ball);
        if(g->ball.vel.y > 0 &&
           SDL_IntersectRectAndLine(
               &paddle, &p1.x, &p1.y, &p2.x, &p2.y))
        {
            beep(36);
            g->ball.vel.x = (moved_ball.x - moved_paddle.x) / 4;
            g->ball.vel.y = -3;
            moved_ball = moving_moved(g->ball);
        }
    }

    g->paddle.pos = moved_paddle;
    g->ball.pos = moved_ball;

    if(g->bricks_left) {
        return 1;
    } else {
        run_scene(breakout_new(g->level+1));
        return 0;
    }
}

static void draw(void* data, Rdr rdr)
{
    Game* g = data;

    SDL_SetRenderDrawColor(rdr, 20, 30, 40, 255);
    const SDL_Rect borders = {0,0,GAME_W,GAME_H};
    SDL_RenderFillRect(rdr, &borders);

    for(int i=g->num_bricks; i-->0;) {
        if(!g->bricks[i]) continue;
        SDL_Rect rect = {
            (i % BRICKS_X) * SPACE_X + PAD/2,
            (i / BRICKS_X) * SPACE_Y + PAD/2,
            SPACE_X - PAD,
            SPACE_Y - PAD,
        };
        SDL_SetRenderDrawColor(rdr, 255, 0, 0, 255);
        SDL_RenderFillRect(rdr, &rect);
    }

    SDL_SetRenderDrawColor(rdr, 255, 255, 255, 255);

    SDL_Rect ball = ball_rect(g);
    SDL_RenderFillRect(rdr, &ball);
    SDL_Rect paddle = paddle_rect(g, 0);
    SDL_RenderFillRect(rdr, &paddle);
}

Scene breakout_new(int level)
{
    Game* game = calloc(1, sizeof(Game));
    game->num_bricks = level * 20 + 30;
    game->bricks_left = game->num_bricks;
    game->ball = (Moving){.vel = {1, 1}};
    game->paddle = (Moving){.pos = {GAME_W/2, GAME_H-20}};
    game->paddle_size = (SDL_Point){10, 4};
    game->stuck = 1;

    game->bricks = malloc(game->num_bricks);
    memset(game->bricks, 1, game->num_bricks);

    sound_level(level+1);

    Scene s = {
        .draw = draw,
        .update = update,
        .data = game,
    };
    return s;
}
