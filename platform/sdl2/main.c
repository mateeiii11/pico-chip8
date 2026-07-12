#include "../../src/chip8.h"
#include "../../roms/ibm_logo.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("CHIP-8 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
    SDL_Renderer *render = SDL_CreateRenderer(window, -1, 0);
    chip8_t chip8;
    chip8_init(&chip8);
    chip8_load_rom(&chip8, ibm_logo_rom, ibm_logo_rom_size);
    while(1)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                SDL_Quit();
                return 0;
            }
        }
        chip8_cycle(&chip8);
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        for(int y = 0; y < SCREEN_HEIGHT; y++)
            for(int x = 0; x < SCREEN_WIDTH; x++)
            {
                if(chip8.screen[y * SCREEN_WIDTH + x] == 1)
                {
                    SDL_Rect rect = {x * 10, y * 10, 10, 10};
                    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
                    SDL_RenderFillRect(render, &rect);
                    
                }
            }
        SDL_RenderPresent(render);
        SDL_Delay(2);
        

    }
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}