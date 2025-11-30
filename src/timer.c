// #pragma once
#include "timer.h"
#include "rcc.h"

void setup_pwm(struct timer* t)
{
    RCC->APB1ENR1 |= BIT(0);
    t->CR1 |= BIT(0) | BIT(1);

    t->CCMR1 &= ~(uint32_t)(0xFF);        // clear OC1 fields
    t->CCMR1 |= (6 << 4);

     // Enable channel 1 output
    t->CCER |= BIT(0);

    // Update generation
    // t->EGR |= BIT(0);
    
    
    t->ARR = 1000;
    t->PSC = 4;
}

void set_duty_cycle(struct timer* t, uint16_t val){
    t->CCR1 = val; 
}