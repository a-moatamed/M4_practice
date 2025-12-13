#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "gpio.h"
#include "helper.h"
#include "rcc.h"
#include "systick.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"


#define BIT(x) (1UL << (x))

static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}