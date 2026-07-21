build:
	gcc -std=c11 src/chip8.c platform/sdl2/main.c -o /tmp/chip8_test -lSDL2

run: build
	SDL_VIDEODRIVER=x11 /tmp/chip8_test