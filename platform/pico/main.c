#include <hardware/gpio.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdio.h>


int main()
{
    stdio_init_all();
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    const uint BUTTON_PIN = 15;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    
    while(1)
    {
        if(gpio_get(BUTTON_PIN) == 0)
        {
            gpio_put(LED_PIN, 1);
            printf("da\n");
        }
        else 
        {
            gpio_put(LED_PIN, 0);
            printf("nu\n");
        }
    }

}