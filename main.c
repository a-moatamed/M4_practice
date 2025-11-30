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
    uint32_t timer = 0, period = 500;

    uint16_t buzzer = PIN('A', 15);
    gpio_set_mode(buzzer, GPIO_MODE_AF);
    gpio_set_af(buzzer, 1); // set alternate function to be 1(tim2 ch1)

    struct timer *t2 = TIM(2);

    // set PWM.
    setup_pwm(t2);

    uint16_t val = 10;

    for (;;)
    {

        if (timer_expired(&timer, period, s_ticks))
        {
            if (val >= 100)
            {
                val = 40;
            }

            val += 10;
            set_duty_cycle(t2, val);
        }
    }
    return 0;
}
