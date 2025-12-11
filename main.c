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

volatile uint32_t s_ticks = 0;


int main(void)
{

    struct  uart* uart = UART1;
    uart_init(uart, 115200);
    {
        /* data */
    };
    
    const uint16_t buzzer = PIN('A', 15); // TIM2_CH1
    const uint16_t led = PIN('B', 10);    // D10 -> TIM2_CH3
    struct timer *tim2 = TIM(2);

    gpio_set_mode(buzzer, GPIO_MODE_AF);
    gpio_set_af(buzzer, 1);  // TIM2 alternate function

    gpio_set_mode(led, GPIO_MODE_AF);
    gpio_set_af(led, 1);     // TIM2 alternate function

    setup_pwm(tim2); // ~1 kHz base, driven from 4 MHz clock

    while (1) {
        for (int step = 0; step <= 100; ++step) {

            uint16_t duty = (uint16_t)(step * 10); // 0..1000 within ARR
            set_duty_cycle(tim2, duty);
            set_duty_cycle_ch3(tim2, duty);
            printf("the pwm value is: %d\r\n", duty);

            spin(40000);                         // ~10 ms at 4 MHz, ~1 s per sweep
        }
    }
}
