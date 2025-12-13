// #pragma once
#include "systick.h"


void SysTick_Handler(void) {
  s_ticks++;
}


// t: expiration time, prd: period, now: current time. Return true if expired
bool timer_expired(uint32_t *t, uint32_t prd, uint32_t now) {
  if (now + prd < *t) *t = 0;                    // Time wrapped? Reset timer
  if (*t == 0) *t = now + prd;                   // First poll? Set expiration
  if (*t > now) return false;                    // Not expired yet, return
  *t = (now - *t) > prd ? now + prd : *t + prd;  // Next expiration time
  return true;                                   // Expired, return true
}


void systick_init(uint32_t ticks){
    if ((ticks - 1) > 0xffffff) return; // maximum value is 24 bit (SYSTICK is 24 bit)
    SYSTICK -> LOAD = ticks - 1;
    SYSTICK -> VAL = 0;
    SYSTICK -> CTRL = BIT(0) | BIT(1) | BIT(2); // enable systick
    RCC -> APB2ENR |= BIT(0); // enable syscfg
}