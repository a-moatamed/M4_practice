// #pragma once
#include "timer.h"
#include "rcc.h"

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