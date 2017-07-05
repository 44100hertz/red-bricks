#include <SDL2/SDL.h>

#define GAME_W 240
#define GAME_H 160
#define BRICKS_Y 4
#define BRICKS_X 10
#define BRICKS (BRICKS_X * BRICKS_Y)
#define PAD 2
#define SPACE_X (GAME_W / BRICKS_X)
#define SPACE_Y (GAME_H / BRICKS_Y / 4)

typedef SDL_Window* Win;
typedef SDL_Renderer* Rdr;

typedef struct {
        SDL_Point pos;
        SDL_Point vel;
} Moving;

typedef struct {
        int bricks[BRICKS];
        Moving paddle;
        SDL_Point paddle_size;
        Moving ball;
        int stuck;
} Game;

SDL_Rect ball_rect(Game* g) {
        const int ball_r = 2;
        SDL_Rect rect = {
                g->ball.pos.x - ball_r,
                g->ball.pos.y - ball_r,
                ball_r*2, ball_r*2,
        };
        return rect;}
SDL_Rect paddle_rect(Game* g, SDL_Point* paddle) {
        if(!paddle) paddle = &g->paddle.pos;
        SDL_Rect rect = {
                paddle->x - g->paddle_size.x,
                paddle->y,
                g->paddle_size.x*2,
                g->paddle_size.y,
        };
        return rect;}
SDL_Point moving_moved(Moving m) {
        return (SDL_Point){
                m.pos.x + m.vel.x, m.pos.y + m.vel.y};}
int brick_pos(SDL_Point pos) {
        return pos.x/SPACE_X + (pos.y/SPACE_Y)*BRICKS_X;}
int brick_on(Game* g, SDL_Point pos) {
        int brick = brick_pos(pos);
        if(brick < 0 || brick >= BRICKS) return 0;
        return g->bricks[brick];}

void game_update(Game* g)
{
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        g->paddle.vel.x *= 0.5;
        if (keys[SDL_SCANCODE_LEFT]) g->paddle.vel.x -= 2;
        if (keys[SDL_SCANCODE_RIGHT]) g->paddle.vel.x += 2;

        if(g->stuck) {
                if (keys[SDL_SCANCODE_SPACE]) {
                        g->stuck = 0;
                        g->ball.vel = (SDL_Point){-1, -1};
                }
                g->paddle.pos = moving_moved(g->paddle);
                g->ball.pos = g->paddle.pos;
                g->ball.pos.y -= 4;
                return;
        }

        SDL_Point moved_ball = moving_moved(g->ball);
        SDL_Point moved_paddle = moving_moved(g->paddle);

        if(moved_ball.x < 0 || moved_ball.x > GAME_W)
                g->ball.vel.x *= -1;
        if(moved_ball.y < 0 || moved_ball.y > GAME_H)
                g->ball.vel.y *= -1;

        int brick = brick_pos(moved_ball);
        int brick_state = brick_on(g, moved_ball);
        if(brick_state) {
                SDL_Point moved_x = {g->ball.pos.x, moved_ball.y};
                SDL_Point moved_y = {moved_ball.x, g->ball.pos.y};
                if(!brick_on(g, moved_x)) g->ball.vel.x *= -1;
                if(!brick_on(g, moved_y)) g->ball.vel.y *= -1;
                g->bricks[brick] = 0;
        } else {
                SDL_Rect paddle = paddle_rect(g, &moved_paddle);
                if(g->ball.vel.y > 0 &&
                   SDL_PointInRect(&moved_ball, &paddle))
                {
                        g->ball.vel.y = -1;
                }
        }

        g->paddle.pos = moved_paddle;
        g->ball.pos = moved_ball;
}

void game_draw(Game* g, Rdr rdr)
{
        for(int i=BRICKS; i-->0;) {
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

Game game_new()
{
        Game game = {
                .ball = {.vel = {1, 1}},
                .paddle = {.pos = {GAME_W/2, GAME_H-20}},
                .paddle_size = {10, 4},
                .stuck = 1,
        };
        memset(game.bricks, 1, sizeof(game.bricks));
        return game;
}

int main()
{
        SDL_Init(SDL_INIT_VIDEO);
        Win win = SDL_CreateWindow("hi", -1, -1, GAME_W*3, GAME_H*3, 0);
        Rdr rdr = SDL_CreateRenderer(win, 0, SDL_RENDERER_PRESENTVSYNC);
        SDL_RenderSetLogicalSize(rdr, GAME_W, GAME_H);

        Game game = game_new();

        SDL_Event e;
        while (1) {
                while (SDL_PollEvent(&e)) {
                        switch(e.type) {
                        case SDL_QUIT: goto quit;
                        }
                }
                SDL_SetRenderDrawColor(rdr, 0, 0, 0, 255);
                SDL_RenderClear(rdr);

                game_update(&game);
                game_draw(&game, rdr);

                SDL_RenderPresent(rdr);
        }
quit:
        SDL_DestroyRenderer(rdr);

        return 0;
}
