// #pragma once
#include "timer.h"



void setup_pwm(struct timer* t)
{
    RCC->APB1ENR1 |= BIT(0);
    t->CR1 = 0;                           // stop and reset key bits

    // CH1 PWM setup (buzzer)
    t->CCMR1 &= ~(uint32_t)(0xFF);        // clear OC1 fields
    t->CCMR1 |= (6UL << 4) | BIT(3);      // PWM mode 1, preload enable
    t->CCER |= BIT(0);                    // enable CH1 output

    // CH3 PWM setup (LED on TIM2_CH3)
    t->CCMR2 &= ~(uint32_t)(0xFF);        // clear OC3 fields
    t->CCMR2 |= (6UL << 4) | BIT(3);      // PWM mode 1, preload enable
    t->CCER |= BIT(8);                    // enable CH3 output

    t->ARR = 1000;
    t->PSC = 4;

    t->EGR |= BIT(0);                     // update registers
    t->CR1 |= BIT(7) | BIT(0);            // ARPE + counter enable
}

void set_duty_cycle(struct timer* t, uint32_t val){ // TODO: choose the channel 
    t->CCR1 = val; 
}
