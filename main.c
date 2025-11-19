#include <inttypes.h>
#include <stdbool.h>
#include "gpio.h"
#include "rcc.h"
#include "systick.h"


volatile uint32_t s_ticks = 0;

int main(void) {
    uint16_t led = PIN('B', 14);
    uint16_t led2 = PIN('B', 2);
    RCC->AHB2ENR |= BIT(PINBANK(led)); // Enable GPIOB clock
    // RCC->AHB2ENR |= BIT(PINBANK(led2)); // Enable GPIOB clock
    gpio_set_mode(led, GPIO_MODE_OUTPUT);
    gpio_set_mode(led2, GPIO_MODE_OUTPUT);
    systick_init(4000000/1000);
    uint32_t timer = 0, period = 500;
    for (;;) {
        if(timer_expired(&timer, period, s_ticks)){
            static bool light;
            static bool light2;
            gpio_write(led, light);
            gpio_write(led2, light2);
            light =! light;
            light2 =! light2;

        }
        
    }
}
