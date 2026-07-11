#ifndef CHIP8_H
#define CHIP8_H


#include <stdint.h>

#define MEMORY_SIZE 4096
#define REGISTER_SIZE 16
#define STACK_SIZE 16
#define KEYS_SIZE 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define PROGRAM_START 0x200
typedef struct 
{
    uint8_t memory[MEMORY_SIZE];
    uint8_t v[REGISTER_SIZE];
    uint8_t timer_delay;
    uint8_t timer_sound;
    uint16_t stack[STACK_SIZE];
    uint16_t I;
    uint16_t pc;
    uint8_t sp;
    uint8_t keys[KEYS_SIZE];
    uint8_t screen[SCREEN_WIDTH * SCREEN_HEIGHT];
} chip8_t;

void chip8_init(chip8_t *chip8);
void chip8_load_rom(chip8_t *chip8, const uint8_t *rom_data, uint16_t rom_size);
void chip8_cycle(chip8_t *chip8);

#endif