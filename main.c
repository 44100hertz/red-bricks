#include <SDL2/SDL.h>
#include "internal.h"
#include "breakout.h"

int main()
{
    run_scene(breakout_new());
    return 0;
}
