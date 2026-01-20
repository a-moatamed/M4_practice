#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "gpio.h"
#include "rcc.h"
#include "systick.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"

#define BIT(x) (1UL << (x))

#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

/**
 * @brief Busy-wait for a short delay.
 * @param count Loop count; larger values wait longer.
 */
static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}



// UART
#define D0  PIN('A', 1)   // RX
#define D1  PIN('A', 0)   // TX

// Digital / PWM / EXTI
#define D2  PIN('D', 14)
#define D3  PIN('B', 0)
#define D4  PIN('A', 3)
#define D5  PIN('B', 4)
#define D6  PIN('B', 1)
#define D7  PIN('A', 4)
#define D8  PIN('B', 2)

// PWM / SPI / TIM
#define D9  PIN('A', 15)
#define D10 PIN('A', 2)
#define D11 PIN('A', 7)
#define D12 PIN('A', 6)
#define D13 PIN('A', 5)

// I2C
#define D14 PIN('B', 9)   // SDA
#define D15 PIN('B', 8)   // SCL


// Analog inputs
#define A0 PIN('C', 5)
#define A1 PIN('C', 4)
#define A2 PIN('C', 3)
#define A3 PIN('C', 2)
#define A4 PIN('C', 1)   // I2C3_SDA
#define A5 PIN('C', 0)   // I2C3_SCL
