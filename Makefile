CC?="gcc"

all:
	$(CC) -g -o game *.c `sdl2-config --cflags --libs`
