CC?="gcc"

all:
	$(CC) -g -o game *.c -lm `sdl2-config --cflags --libs`
