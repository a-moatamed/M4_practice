#pragma once

#include "helper.h"


#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)
#define GPIO(bank) ((struct gpio *) (0x48000000 + 0x400 * (bank))) 



struct gpio {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2], BRR, ASCR;

};


enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

void gpio_set_mode(uint16_t pin, uint8_t mode);
void gpio_write(uint16_t pin, bool val);
void gpio_set_af(uint16_t pin, uint8_t af_num);
