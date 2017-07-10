#include <SDL2/SDL.h>
#include "internal.h"
#include "menu.h"

int main()
{
    run_scene(menu_new());
    return 0;
}
