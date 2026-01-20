#pragma once

#include "helper.h"

#define GPIO(bank) ((struct gpio *) (0x48000000 + 0x400 * (bank)))

// GPIO register map.
struct gpio {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2], BRR, ASCR;
};

// GPIO mode values for MODER.
enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

/**
 * @brief Configure a GPIO pin mode.
 * @param pin Encoded pin ID (PORT + pin number).
 * @param mode GPIO mode value (GPIO_MODE_*).
 */
void gpio_set_mode(uint16_t pin, uint8_t mode);
/**
 * @brief Write a logic level to a GPIO pin.
 * @param pin Encoded pin ID (PORT + pin number).
 * @param val true for high, false for low.
 */
void gpio_write(uint16_t pin, bool val);
/**
 * @brief Set the alternate function for a GPIO pin.
 * @param pin Encoded pin ID (PORT + pin number).
 * @param af_num Alternate function number.
 */
void gpio_set_af(uint16_t pin, uint8_t af_num);
