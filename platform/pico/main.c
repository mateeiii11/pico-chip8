#include <hardware/gpio.h>
#include <hardware/regs/addressmap.h>
#include <hardware/structs/resets.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include "spi.h"
#include <hardware/spi.h>
#include "ili9341_init.h"
#include "../../../chip8.h"
#include "ibm_logo.h"
#include "../../../pong_data.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void send_comm(uint8_t a)
{
    gpio_put(CS, 0);
    gpio_put(DC, 0);
    spi_write_blocking(spi0, &a, 1);
    gpio_put(CS, 1);
}

void send_data(uint8_t a)
{
    gpio_put(CS, 0);
    gpio_put(DC, 1);
    spi_write_blocking(spi0, &a, 1);
    gpio_put(CS, 1);
}

void init_cmds()
{
    for(int i = 0; cmds[i] != 0xFF; )
    {
        send_comm(cmds[i]);
        i++;
        int n = cmds[i];
        for(int j = 1; j <= n; j++)
            send_data(cmds[i + j]);
        i += (n + 1);
    }
}
void init_gp()
{
    spi_init(spi0, 5000000);
    gpio_set_function(SDI, GPIO_FUNC_SPI);
    gpio_set_function(SCK, GPIO_FUNC_SPI);
    gpio_set_function(SDO, GPIO_FUNC_SPI);

    gpio_init(CS);
    gpio_init(DC);
    gpio_init(RESET);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_set_dir(RESET, GPIO_OUT);
    gpio_set_dir(DC, GPIO_OUT); 

    gpio_put(RESET, 0);
    sleep_ms(10);
    gpio_put(RESET, 1);
    sleep_ms(1000);
}
void set_screen_color()
{
    send_comm(0x2A);
    send_data(0x0);
    send_data(0x0);
    send_data(239 >> 8);
    send_data(239 & 0x00FF);

    send_comm(0x2B);
    send_data(0x0);
    send_data(0x0);
    send_data(319 >> 8);
    send_data(319 & 0x00FF);

    send_comm(0x2C);
    for(int i = 1; i <= 76800; i++)
    {
        send_data(0);
        send_data(0);
    }
}

void draw_pixel(int x, int y, uint16_t color)
{
    send_comm(0x2A);
    send_data(x >> 8);
    send_data(x & 0x00FF);
    send_data(x >> 8);
    send_data(x & 0x00FF);

    send_comm(0x2B);
    send_data(y >> 8);
    send_data(y & 0x00FF);
    send_data(y >> 8);
    send_data(y & 0x00FF);

    send_comm(0x2C);
    send_data(color >> 8);
    send_data(color & 0x00FF);

}

int main()
{
    stdio_init_all();
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    const uint BUTTON_LEFT_UP = 15;
    uint8_t previousScreen[SCREEN_WIDTH * SCREEN_HEIGHT];
    chip8_t chip8;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(BUTTON_LEFT_UP);
    gpio_set_dir(BUTTON_LEFT_UP, GPIO_IN);
    gpio_pull_up(BUTTON_LEFT_UP);
    init_gp();
    init_cmds();
    set_screen_color();
    memset(previousScreen, 0, sizeof(previousScreen));
    chip8_init(&chip8);
    chip8_load_rom(&chip8, pong_rom, pong_rom_size);
    uint32_t lastUpdate = to_ms_since_boot(get_absolute_time());
    while(1)
    {
        gpio_put(LED_PIN, 1);
        uint32_t currentTime = to_ms_since_boot(get_absolute_time());
        if(currentTime - lastUpdate >= 16)
        {
            if(chip8.timer_delay > 0)
                chip8.timer_delay--;
            if(chip8.timer_sound > 0)
                chip8.timer_sound--;
            lastUpdate = currentTime;
        }
        chip8_cycle(&chip8);
        for(int y = 0; y < SCREEN_HEIGHT; y++)
            for(int x = 0; x < SCREEN_WIDTH; x++)
            {
                if(chip8.screen[y * SCREEN_WIDTH + x] != previousScreen[y * SCREEN_WIDTH + x])
                {
                    if(chip8.screen[y * SCREEN_WIDTH + x] == 1)
                    {
                        int xv = x * 3; 
                        int yv = y * 10;
                        for(int dx = 0; dx < 3; dx++)
                            for(int dy = 0; dy < 10; dy++)  
                            {
                                draw_pixel(xv + dx, yv + dy, 0xFFFF);
                            }
                    }
                    else
                    {
                        int xv = x * 3; 
                        int yv = y * 10;
                        for(int dx = 0; dx < 3; dx++)
                            for(int dy = 0; dy < 10; dy++)
                            {
                                draw_pixel(xv + dx, yv + dy, 0x0000);
                            }
                    }
                    
                }
                
                
            }
        memcpy(previousScreen, chip8.screen, sizeof(previousScreen));
        if(gpio_get(BUTTON_LEFT_UP) == 0)
        {
            chip8.keys[0x1] = 1;
        }
        else
        {
            chip8.keys[0x1] = 0;
        }

    }

}