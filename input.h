//#include <SDL2/SDL.h>
void input_event(SDL_Event e);

typedef struct {
    char keys[SDL_NUM_SCANCODES];
} Input;

Input input_update();
