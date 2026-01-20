#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include "rcc.h"

// General purpose timers.
#define TIM(x) ((struct timer *)(0x40000000 + 0x400 * (x - 2)))

// Timer register map.
struct timer
{
    volatile uint32_t CR1, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2,
        CCER, CNT, PSC, ARR, RCR, CCR1, CCR2, CCR3, CCR4, BDTR, DCR, DMAR;
};

/**
 * @brief Configure a timer for PWM output.
 * @param t Timer instance.
 */
void setup_pwm(struct timer *t);
/**
 * @brief Set PWM duty cycle.
 * @param t Timer instance.
 * @param val Duty cycle in timer ticks (0-1000).
 */
void set_duty_cycle(struct timer *t, uint32_t val);
