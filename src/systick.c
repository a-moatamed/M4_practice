#include "systick.h"


void SysTick_Handler(void) {
  s_ticks++;
}


bool timer_expired(uint32_t *t, uint32_t prd, uint32_t now) {
  if (now + prd < *t) *t = 0;
  if (*t == 0) *t = now + prd;
  if (*t > now) return false;
  *t = (now - *t) > prd ? now + prd : *t + prd;
  return true;
}


void systick_init(uint32_t ticks){
    if ((ticks - 1) > 0xffffff) return;
    SYSTICK -> LOAD = ticks - 1;
    SYSTICK -> VAL = 0;
    SYSTICK -> CTRL = BIT(0) | BIT(1) | BIT(2);
    RCC -> APB2ENR |= BIT(0);
}
