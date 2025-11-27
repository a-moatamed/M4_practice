#pragma once
#define BIT(x) (1UL << (x))

static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}