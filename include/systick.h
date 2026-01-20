#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include "rcc.h"
#include "helper.h"

#define SYSTICK ((struct systick *) 0xe000e010)

// SysTick register map.
struct systick{
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
};

// Global millisecond tick counter.
extern volatile uint32_t s_ticks;

/**
 * @brief SysTick interrupt handler.
 *
 * Increments the global tick counter.
 */
void SysTick_Handler(void);
/**
 * @brief Check whether a period has elapsed since the last expiry.
 * @param t Pointer to the next expiration time (maintained by the helper).
 * @param prd Period in ticks.
 * @param now Current tick time.
 * @return true when the period has elapsed.
 */
bool timer_expired(uint32_t *t, uint32_t prd, uint32_t now);
/**
 * @brief Initialize the SysTick timer.
 * @param ticks Reload value for the desired tick interval.
 */
void systick_init(uint32_t ticks);
