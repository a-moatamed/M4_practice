#include <inttypes.h>
#include <stdbool.h>
#include "rcc.h"
#include "helper.h"

#define SYSTICK ((struct systick *) 0xe000e010)


struct systick{
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
};



extern volatile uint32_t s_ticks; // volatile is important!!

void SysTick_Handler(void);
bool timer_expired(uint32_t *t, uint32_t prd, uint32_t now);

static inline void systick_init(uint32_t ticks){
    if ((ticks - 1) > 0xffffff) return; // maximum value is 24 bit (SYSTICK is 24 bit)
    SYSTICK -> LOAD = ticks - 1;
    SYSTICK -> VAL = 0;
    SYSTICK -> CTRL = BIT(0) | BIT(1) | BIT(2); // enable systick
    RCC -> APB2ENR |= BIT(0); // enable syscfg
}