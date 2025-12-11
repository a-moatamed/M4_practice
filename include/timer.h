#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include "rcc.h"

// gernal puprose timers.
#define TIM(x) ((struct timer *)(0x40000000 + 0x400 * (x - 2)))

struct timer
{
    volatile uint32_t CR1, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2,
        CCER, CNT, PSC, ARR, RCR, CCR1, CCR2, CCR3, CCR4, BDTR, DCR, DMAR;
};


static inline void setup_pwm(struct timer* t)
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

static inline void set_duty_cycle(struct timer* t, uint16_t val){
    t->CCR1 = val; 
}

static inline void set_duty_cycle_ch3(struct timer* t, uint16_t val){
    t->CCR3 = val;
}
