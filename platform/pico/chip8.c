#include "chip8.h"
#include <stdint.h>
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
static const uint8_t chip8_fontset[80] = {
     0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F

};

void chip8_init(chip8_t *chip8)
{
    memset(chip8, 0 , sizeof(chip8_t));
    chip8->pc = PROGRAM_START;
    memcpy(chip8->memory, chip8_fontset, sizeof(chip8_fontset));

}

void chip8_load_rom(chip8_t *chip8, const uint8_t *rom_data, uint16_t rom_size)
{
    memcpy(chip8->memory + PROGRAM_START ,rom_data, rom_size);
}

void chip8_cycle(chip8_t *chip8)
{
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    switch(((opcode) & 0xF000) >> 12)
    {
        case 0x0:
        {
            if(opcode == 0x00E0)
            {
                memset(chip8->screen, 0, sizeof(chip8->screen));
                chip8->pc += 2;
            }
            else if(opcode == 0x00EE)
            {
                chip8->pc = chip8->stack[chip8->sp];
                chip8->sp--;
            }
            break;
        }
        case 0x1:
            chip8->pc = (opcode & 0x0FFF);
            break;
        case 0x2:
        {
            chip8->stack[++chip8->sp] = chip8->pc + 2;
            chip8->pc = (opcode & 0x0FFF);
            break;
        }
        case 0x3:
            if(chip8->v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                chip8->pc += 4;
            else 
                chip8->pc += 2;
            break;
        case 0x4:
            if(chip8->v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                chip8->pc += 4;
            else
                chip8->pc += 2;
            break;
        case 0x5:
            if(chip8->v[(opcode & 0x0F00) >> 8] == chip8->v[(opcode & 0x00F0) >> 4])
                chip8->pc += 4;
            else
                chip8->pc += 2;
            break;
        case 0x6:
            chip8->v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            chip8->pc += 2;
            break;
        case 0x7:
            chip8->v[(opcode & 0x0F00) >> 8] += (opcode & 0x0FF);
            chip8->pc += 2;
            break;
        case 0x8:
        {
            uint8_t x = ((opcode & 0x0F00) >> 8);
            uint8_t y = ((opcode & 0x00F0) >> 4);
            switch((opcode & 0x000F))
            {
                case 0x0:
                    chip8->v[x] = chip8->v[y];
                    break;
                case 0x1:
                    chip8->v[x] = (chip8->v[x] | chip8->v[y]);
                    break;
                case 0x2:
                    chip8->v[x] = (chip8->v[x] & chip8->v[y]);
                    break;
                case 0x3:
                    chip8->v[x] = (chip8->v[x] ^ chip8->v[y]);
                    break;
                case 0x4:
                    if(chip8->v[x] + chip8->v[y] > 255)
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    chip8->v[x] = (chip8->v[x] + chip8->v[y]) & 0xFF;
                    break;
                case 0x5:
                    if(chip8->v[x] >= chip8->v[y])
                        chip8->v[0xF] = 1;
                    else 
                        chip8->v[0xF] = 0;
                    chip8->v[x] = chip8->v[x] - chip8->v[y];
                    break;
                case 0x6:
                    if((chip8->v[x] & 1) == 0x1)
                       chip8->v[0xF] = 1;
                    else 
                        chip8->v[0xF] = 0;
                    chip8->v[x] /= 2;
                    break;
                case 0x7:
                    if(chip8->v[y] >= chip8->v[x])
                        chip8->v[0xF] = 1;
                    else 
                        chip8->v[0xF] = 0;
                    chip8->v[x] = chip8->v[y] - chip8->v[x];
                    break;
                case 0xE:
                    if(((chip8->v[x] & (1 << 7)) >> 7) == 0x1)
                       chip8->v[0xF] = 1;
                    else 
                        chip8->v[0xF] = 0;
                    chip8->v[x] *= 2;
                    break;
                default:
                    break;
            }
            chip8->pc += 2;

        }
            break;
        case 0x9:
            if(chip8->v[(opcode & 0x0F00) >> 8] != chip8->v[(opcode & 0x00F0) >> 4])
                chip8->pc += 4;
            else
                chip8->pc += 2;
            break;
        case 0xA:
            chip8->I = (opcode & 0x0FFF);
            chip8->pc += 2;
            break;
        case 0xB:
            chip8->pc = ((opcode & 0x0FFF) + chip8->v[0]);
            break;
        case 0xC:
            chip8->v[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);  
            chip8->pc += 2;
            break;
        case 0xD:
        {
            uint8_t X = (opcode & 0x0F00) >> 8;
            uint8_t Y = (opcode & 0x00F0) >> 4;
            uint8_t n = opcode & 0x000F;
            chip8->v[0xF] = 0;
            for(uint8_t linie = 0; linie < n; linie++)
            {
                uint8_t curr = chip8->memory[chip8->I + linie];
                for(uint8_t i = 0; i < 8; i++)
                {
                    uint8_t bit = (curr & (1 << (7 - i))) >> (7 - i);
                    uint8_t x = chip8->v[X] + i;
                    uint8_t y = chip8->v[Y] + linie;
                    x %= SCREEN_WIDTH;
                    y %= SCREEN_HEIGHT;
                    uint16_t index = y * SCREEN_WIDTH + x;
                    if(chip8->screen[index] == 0x1 && bit == 0x1)
                        chip8->v[0xF] = 1;
                    chip8->screen[index] = chip8->screen[index] ^ bit;
                }
            }
            chip8->pc += 2;
                break;
        }
        case 0xE:
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            if((opcode & 0x00FF) == 0x9E)
            {
                if(chip8->keys[chip8->v[x]] == 1)
                    chip8->pc += 4;
                else
                    chip8->pc += 2;
            }
            else if((opcode & 0x00FF) == 0xA1)
            {
                if(chip8->keys[chip8->v[x]] == 0)
                    chip8->pc += 4;
                else
                    chip8->pc += 2;
            }
            break;
        }
        case 0xF:
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            switch(opcode & 0x00FF)
            {
                case 0x07:
                    chip8->v[x] = chip8->timer_delay;
                    chip8->pc += 2; 
                    break;
                case 0x15:
                    chip8->timer_delay = chip8->v[x];
                    chip8->pc += 2; 
                    break;
                case 0x18:
                    chip8->timer_sound = chip8->v[x];
                    chip8->pc += 2; 
                    break;
                case 0x1E:
                    chip8->I += chip8->v[x];
                    chip8->pc += 2; 
                    break;
                case 0x29:
                    chip8->I = chip8->v[x] * 5;
                    chip8->pc += 2; 
                    break;
                case 0x33:
                    chip8->memory[chip8->I] = chip8->v[x] / 100;
                    chip8->memory[chip8->I + 1] = (chip8->v[x] / 10) % 10;
                    chip8->memory[chip8->I + 2] = (chip8->v[x]) % 10;
                    chip8->pc += 2; 
                    break;
                case 0x55:
                    for(int i = 0; i <= x; i++)
                        chip8->memory[chip8->I + i] = chip8->v[i]; 
                    chip8->pc += 2; 
                    break;
                case 0x65:
                    for(int i = 0; i <= x; i++)
                         chip8->v[i] = chip8->memory[chip8->I + i]; 
                    chip8->pc += 2; 
                    break;
                case 0x0A:
                {
                    printf("Waiting for key, V[%d]\n", x);
                    for(int i = 0; i < 16; i++)
                        if(chip8->keys[i] == 1)
                        {
                            chip8->v[x] = i;
                            chip8->pc += 2; 
                            break;
                        }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}