// #pragma once
#include <inttypes.h>
#include <stdbool.h>
#include "gpio.h"
#include "rcc.h"
#include "systick.h"
#include "helper.h"
#include "timer.h"
#include "uart.h"

volatile uint32_t s_ticks = 0;

int main(void)
{
    systick_init(4000000 / 1000); // Tick every 1 ms
    uint32_t timer = 0, period = 1000;

    uint16_t led = PIN('A', 15);      
    gpio_set_mode(led, GPIO_MODE_OUTPUT); 
   

    struct uart * uart = UART1;
    uart_init(uart, 115200); // Initialise UART

    uart_write_buf(uart, "START\r\n", 7);


    for (;;)
    {

        if (timer_expired(&timer, period, s_ticks))
        {
            static bool on;                     // This block is executed
            gpio_write(led, on);                // Every `period` milliseconds
            on = !on;                           // Toggle LED state
            uart_write_buf(uart, "hi\r\n", 4); // Write message
        }
        
    }
    return 0;
}
