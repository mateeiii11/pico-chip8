#include "../../src/chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>


int map_key(SDL_Keycode key)
{
    switch(key)
    {
        case SDLK_1:
            return 0x1;
            break;
        case SDLK_2:
            return 0x2;
            break;
        case SDLK_3:
            return 0x3; 
            break;
        case SDLK_4:
            return 0xC;
            break;
        case SDLK_q:
            return 0x4;
            break;
        case SDLK_w:
            return 0x5;
            break;
        case SDLK_e:
            return 0x6; 
            break;
        case SDLK_r:
            return 0xD;
            break;
        case SDLK_a:
            return 0x7;
            break;
        case SDLK_s:
            return 0x8;
            break;
        case SDLK_d:
            return 0x9; 
            break;
        case SDLK_f:
            return 0xE;
            break;
        case SDLK_z:
            return 0xA;
            break;
        case SDLK_x:
            return 0x0;
            break;
        case SDLK_c:
            return 0xB; 
            break;
        case SDLK_v:
            return 0xf;
            break;
        default:
            return -1;
        
    }
}

uint8_t* load_rom_file(const char *filename, uint16_t* rom_size)
{
    FILE *f = fopen(filename, "rb");
    if(f == NULL)
        return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *buffer = malloc(size);
    fread(buffer, 1, size, f);
    fclose(f);
    *rom_size = (uint16_t)size;
    return buffer;
}
int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("CHIP-8 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
    SDL_Renderer *render = SDL_CreateRenderer(window, -1, 0);
    chip8_t chip8;

    chip8_init(&chip8);

    uint16_t rom_size;
    uint8_t *romData = load_rom_file("roms/Pong.ch8", &rom_size);
    if(romData == NULL)
        return 1;
    chip8_load_rom(&chip8, romData, rom_size);
    Uint32 lastUpdate = SDL_GetTicks();
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
            else if(map_key(event.key.keysym.sym) != -1 && event.type == SDL_KEYDOWN)
            {
                printf("Type: %s, repeat: %d\n", event.type == SDL_KEYDOWN ? "DOWN" : "UP", event.key.repeat);
                //printf("Key pressed down is %d\n", event.key.keysym.sym);
                chip8.keys[map_key(event.key.keysym.sym)] = 1;
            }
            else if(map_key(event.key.keysym.sym) != -1 && event.type == SDL_KEYUP)
            {
                printf("Type: %s, repeat: %d\n", event.type == SDL_KEYDOWN ? "DOWN" : "UP", event.key.repeat);
                //printf("Key pressed up is %d", event.key.keysym.sym);
                chip8.keys[map_key(event.key.keysym.sym)] = 0;
            }
        }
        Uint32 currentTime = SDL_GetTicks();
        if(currentTime - lastUpdate >= 16)
        {
            if(chip8.timer_delay > 0)
                chip8.timer_delay--;
            if(chip8.timer_sound > 0)
                chip8.timer_sound--;
            lastUpdate = currentTime;
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