#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gpio.h"
#include "helper.h"
#include "rcc.h"
#include "systick.h"
#include "timer.h"
#include "adc.h"
#include "uart.h"

#include <stdio.h>


#define CLOCK 4000000

volatile uint32_t s_ticks = 0;


/**
 * the main of this function is to to toggle a led.
 * on-board led(LED2: the green led) is connected to the pin PB14.
 */
int main(void)
{
    // intialize the systick
    systick_init(CLOCK/1000);
    uint32_t timer = 0;
    uint32_t period = 500;


    // define the led pin
    uint16_t led = PIN('B', 14);
    // choose the mode ### note that the function will enable the RCC port 
    gpio_set_mode(led, GPIO_MODE_OUTPUT);
    static bool on = true; // set the flag of the led.

    struct uart* uart = UART1;
    uart_init(uart, 115200);
    

    
    while (1)
    {
        if(timer_expired(&timer, period, s_ticks))
        {

            on = !on;
            gpio_write(led, on);
            printf("LED: %d, tick: %lu\r\n", on, s_ticks);  // Write message
    
        }

        
    }
}
