#pragma once
#include "rcc.h"
#include "gpio.h"
#include "helper.h"
#include <inttypes.h>
#include <stdlib.h>

#define UART1 ((struct uart *)0x40013800)
#define UART2 ((struct uart *)0x40004400)
#define UART3 ((struct uart *)0x40004800)
#define UART4 ((struct uart *)0x40004C00)
#define UART5 ((struct uart *)0x40005000)

#define FREQ 4000000

struct uart
{
    volatile uint32_t CR1, CR2, CR3, BRR, GTPR, GTOR, RQR, ISR, ICR, RDR, TDR;
};

static inline int uart_read_ready(struct uart *uart)
{
    return uart->ISR & BIT(5); // If RXNE bit is set, data is ready
}

void uart_init(struct uart *uart, uint32_t baud);

static inline uint8_t uart_read_byte(struct uart *uart)
{
    return (uint8_t)(uart->RDR & 255);
}

static inline void uart_write_byte(struct uart *uart, uint8_t byte)
{
    uart->TDR = byte;
    while ((uart->ISR & BIT(7)) == 0)
        spin(1);
}

static inline void uart_write_buf(struct uart *uart, char *buf, size_t len)
{
    while (len-- > 0)
        uart_write_byte(uart, *(uint8_t *)buf++);
}