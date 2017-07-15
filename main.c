#include <SDL2/SDL.h>
#include "internal.h"
#include "menu.h"

int main(int argc, char** argv)
{
    run_scene(menu_new());
    return 0;
}
