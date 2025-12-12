#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gpio.h"
#include "helper.h"
#include "rcc.h"
#include "systick.h"
#include "timer.h"
#include "uart.h"
#include <stdio.h>

#define CLOCK 4000000

void led_bar(uint16_t duty_cycle);

// define leds
uint16_t led1 = PIN('A', 4);  // D7
uint16_t led2 = PIN('B', 1);  // D6
uint16_t led3 = PIN('B', 4);  // D5
uint16_t led4 = PIN('A', 3);  // D4
uint16_t led5 = PIN('B', 0);  // D3
uint16_t led6 = PIN('D', 14); // D2

volatile uint32_t s_ticks = 0;

int main(void)
{

    // define time variables
    // uint32_t t = 0;
    // uint32_t period = 500;

    // systick_init(CLOCK / 1000);

    

    // choose the mode for the output
    gpio_set_mode(led1, GPIO_MODE_OUTPUT);
    gpio_set_mode(led2, GPIO_MODE_OUTPUT);
    gpio_set_mode(led3, GPIO_MODE_OUTPUT);
    gpio_set_mode(led4, GPIO_MODE_OUTPUT);
    gpio_set_mode(led5, GPIO_MODE_OUTPUT);
    gpio_set_mode(led6, GPIO_MODE_OUTPUT);

    // bool on = true;

    uint16_t duty_cycle = 80;

    while (1)
    {
        led_bar(duty_cycle);
    }
}

void led_bar(uint16_t duty_cycle)
{
    if (duty_cycle >= 90)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, true);
    }
    else if(duty_cycle >= 80){
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, false);

    }
    else if(duty_cycle >= 70){
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, false);
        gpio_write(led6, false);

    }
    else if(duty_cycle >= 60){
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);

    }
    else if(duty_cycle >= 50){
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);

    }
    else if(duty_cycle >= 40){
        gpio_write(led1, true);
        gpio_write(led2, false);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);

    }
    else {
        gpio_write(led1, false);
        gpio_write(led2, false);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);

    }
}
