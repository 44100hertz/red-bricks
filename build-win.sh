#!/bin/sh

i686-w64-mingw32-gcc -g -o game.exe *.c -lm `i686-w64-mingw32-pkg-config sdl2 --cflags --libs`

if [ ! -f SDL2.dll ]; then
    wget https://www.libsdl.org/release/SDL2-2.0.5-win32-x86.zip
    unzip SDL2-2.0.5-win32-x86.zip
fi

zip win *
