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

void setup_pwm(struct timer* t);
void set_duty_cycle(struct timer* t, uint32_t val);
