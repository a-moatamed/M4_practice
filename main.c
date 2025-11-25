#include <inttypes.h>
#include <stdbool.h>
#include "gpio.h"
#include "rcc.h"
#include "systick.h"
#include "helper.h"

// gernal puprose timers.
#define TIM(x) ((struct timer *)(0x40000000 + 0x400 * (x - 2)))

struct timer
{
    volatile uint32_t CR1, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2,
        CCER, CNT, PSC, ARR, RCR, CCR1, CCR2, CCR3, CCR4, BDTR, DCR, DMAR;
};

volatile uint32_t s_ticks = 0;

void setup_pwm(struct timer* t2)
{
    RCC->APB1ENR1 |= BIT(0);
    t2->CR1 |= BIT(0) | BIT(1);

    t2->CCMR1 &= ~(uint32_t)(0xFF);        // clear OC1 fields
    t2->CCMR1 |= (6 << 4);

     // Enable channel 1 output
    t2->CCER |= BIT(0);

    // Update generation
    // t2->EGR |= BIT(0);
    
    
    t2->ARR = 1000;
    t2->PSC = 4;
}

void set_duty_cycle(struct timer* t2, uint16_t val){
    t2->CCR1 = val; 
}


int main(void)
{
    uint16_t led = PIN('A', 15);       // Blue LED
    RCC->AHB2ENR |= BIT(PINBANK(led)); // Enable GPIO clock for LED

    systick_init(4000000 / 1000); // Tick every 1 ms

    gpio_set_mode(led, GPIO_MODE_AF); // Set blue LED to output mode
    struct gpio* gpioa = GPIO(PINBANK(led));
    gpioa->AFRH |= BIT(28);

    struct timer* t2 = TIM(2);
    setup_pwm(t2);

    uint16_t value = 0;
    set_duty_cycle(t2, value);


    uint32_t timer = 0, period = 10; // Declare timer and 500ms period
    for (;;)
    {
        
        if (timer_expired(&timer, period, s_ticks))
        {
            value += 10;
            if (value >= 1000){
                for (;;){
                    value -=10;
                    set_duty_cycle(t2, value);
                    if(value <=0 ) break;
                }
            };
            set_duty_cycle(t2, value);
        }
        // Here we could perform other activities!
    }
    return 0;
}


/**
 * setup_pwm(){
 *  enable timer2 prepherial
 * set timer mode
 * set the prescaler
 * set alignment option
 * set direction
 * set OC register
 * enable counter
 * enable output compare
 * choose the prescaler
 * set the ARR (the frequency)
 * set OC value (the duty cycle)
 *
 * }
 */