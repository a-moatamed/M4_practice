#include "timer.h"

void setup_pwm(struct timer *t)
{
    RCC->APB1ENR1 |= BIT(0);
    t->CR1 = 0;

    // PWM channels
    t->CCMR1 &= ~(uint32_t)(0xFF);
    t->CCMR1 |= (6UL << 4) | BIT(3);
    t->CCER |= BIT(0);

    t->CCMR2 &= ~(uint32_t)(0xFF);
    t->CCMR2 |= (6UL << 4) | BIT(3);
    t->CCER |= BIT(8);

    t->ARR = 1000;
    t->PSC = 4;

    t->EGR |= BIT(0);
    t->CR1 |= BIT(7) | BIT(0);
}

void set_duty_cycle(struct timer *t, uint32_t val) {
    t->CCR1 = val;
}
