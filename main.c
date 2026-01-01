// #pragma once
#include "helper.h"


#define CLOCK 4000000

void led_bar(uint32_t duty_cycle);


// define leds
uint16_t led1 = D0;
uint16_t led2 = D1;
uint16_t led3 = D2;
uint16_t led4 = D3;
uint16_t led5 = D4;
uint16_t led6 = D5;
uint16_t led7 = D6;
uint16_t led8 = D7;
uint16_t led9 = D8;
uint16_t led10 = D10;


volatile uint32_t s_ticks = 0;

int main(void)
{
    // initialize UART
    uart_init(UART1, 115200); // set up PB6/PB7 as TX/RX and enable UART1    

    // setup the ADC
    uint16_t pot = A0;
    gpio_set_mode(pot, GPIO_MODE_ANALOG);
        adc_init(pot);
        spin(100000);   // REQUIRED: allow ADC to stabilize


    // set up buzzer
    const uint16_t buzzer = D9; // TIM2_CH1
    struct timer *tim2 = TIM(2);
    gpio_set_mode(buzzer, GPIO_MODE_AF);
    gpio_set_af(buzzer, 1); // TIM2 alternate function
    setup_pwm(tim2);        // ~1 kHz base, driven from 4 MHz clock

    // choose the mode for the output
    gpio_set_mode(led1, GPIO_MODE_OUTPUT);
    gpio_set_mode(led2, GPIO_MODE_OUTPUT);
    gpio_set_mode(led3, GPIO_MODE_OUTPUT);
    gpio_set_mode(led4, GPIO_MODE_OUTPUT);
    gpio_set_mode(led5, GPIO_MODE_OUTPUT);
    gpio_set_mode(led6, GPIO_MODE_OUTPUT);

    uint32_t duty_cycle = 0;

    while (1)
    {

        duty_cycle = (uint32_t)((adc_read_avg(16) * 100) / 4095); // average 16 samples to smooth noise

        led_bar(duty_cycle);
        set_duty_cycle(tim2, duty_cycle);
        printf("DUTY CYCLE VALUE: %lu\r\n", duty_cycle);
        spin(20000); // simple throttle to avoid flooding the UART
    }
}

void led_bar(uint32_t duty_cycle)
{
    if (duty_cycle >= 90)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, true);
        gpio_write(led7, true);
        gpio_write(led8, true);
        gpio_write(led9, true);
        gpio_write(led10, true);
    }
    else if (duty_cycle >= 80)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 70)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 60)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 50)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 40)
    {
        gpio_write(led1, true);
        gpio_write(led2, false);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else
    {
        gpio_write(led1, false);
        gpio_write(led2, false);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
}
