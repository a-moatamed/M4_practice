#include <inttypes.h>
#include <stdbool.h>
#include "gpio.h"
#include "rcc.h"
#include "systick.h"


volatile uint32_t s_ticks = 0;

int main(void) {
    uint16_t led = PIN('B', 14);
    uint16_t led2 = PIN('B', 2);
    RCC->AHB2ENR |= BIT(PINBANK(led)); // Enable GPIOB clock
    // RCC->AHB2ENR |= BIT(PINBANK(led2)); // Enable GPIOB clock
    gpio_set_mode(led, GPIO_MODE_OUTPUT);
    gpio_set_mode(led2, GPIO_MODE_OUTPUT);
    systick_init(4000000/1000);
    uint32_t timer = 0, period = 500;
    for (;;) {
        if(timer_expired(&timer, period, s_ticks)){
            static bool light;
            static bool light2;
            gpio_write(led, light);
            gpio_write(led2, light2);
            light =! light;
            light2 =! light2;

        }
        
    }
}

__attribute__((naked, noreturn)) void _reset(void) {
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;
    main();
    for (;;) (void)0;
}

extern void _reset(void);
extern unsigned long _estack;
__attribute__((section(".vectors"))) 
void (*const tab[16 + 91])(void) = {
    (void (*)(void))(&_estack), _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler};
