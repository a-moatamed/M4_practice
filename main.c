#include <inttypes.h>
#include <stdbool.h>
#include "gpio.h"
#include "rcc.h"
#include "systick.h"


volatile uint32_t s_ticks = 0;

int main(void) {

    // define the pin
    uint16_t led1 = PIN('A', 5);
    uint16_t led2 = PIN('A', 6);
    uint16_t led3 = PIN('A', 7);
    uint16_t led4 = PIN('A', 2);
    uint16_t led5 = PIN('A', 15);
    uint16_t led6 = PIN('B', 2);
    uint16_t led7 = PIN('A', 4);
    uint16_t led8 = PIN('B', 1);
    uint16_t led9 = PIN('B', 4);
    uint16_t led10= PIN('B', 9);

    // enable the GPIO PORT in RCC
    RCC -> AHB2ENR |= BIT(PINBANK(led1));
    RCC -> AHB2ENR |= BIT(PINBANK(led6));

    // set the mode for the pin
    gpio_set_mode(led1  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led2  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led3  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led4  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led5  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led6  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led7  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led8  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led9  , GPIO_MODE_OUTPUT);
    gpio_set_mode(led10 , GPIO_MODE_OUTPUT);
    
    
    systick_init(4000000/1000);

    uint32_t timer = 0, period = 500;
    for (;;) {
        if(timer_expired(&timer, period, s_ticks)){
            static bool light1 ;
            static bool light2 ;
            static bool light3 ;
            static bool light4 ;
            static bool light5 ;
            static bool light6 ;
            static bool light7 ;
            static bool light8 ;
            static bool light9 ;
            static bool light10;

            gpio_write(led1, light1);
            gpio_write(led2, light2);
            gpio_write(led3, light3);
            gpio_write(led4, light4);
            gpio_write(led5, light5);
            gpio_write(led6, light6);
            gpio_write(led7, light7);
            gpio_write(led8, light8);
            gpio_write(led9, light9);
            gpio_write(led1, light1);
            
            light1  =! light1 ;
            light2  =! light2 ;
            light3  =! light3 ;
            light4  =! light4 ;
            light5  =! light5 ;
            light6  =! light6 ;
            light7  =! light7 ;
            light8  =! light8 ;
            light9  =! light9 ;
            light10 =! light10;

        }
        
    }
}



