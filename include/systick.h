#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include "rcc.h"
#include "helper.h"

#define SYSTICK ((struct systick *) 0xe000e010)

struct systick{
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
};

extern volatile uint32_t s_ticks;

// SysTick interrupt handler increments the tick counter.
void SysTick_Handler(void);
// Return true when the period has elapsed since the last expiry.
bool timer_expired(uint32_t *t, uint32_t prd, uint32_t now);
// Initialize SysTick with the given reload value.
void systick_init(uint32_t ticks);
