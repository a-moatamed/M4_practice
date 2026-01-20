#pragma once
#include "helper.h"
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

// Return non-zero when a byte can be read.
int uart_read_ready(struct uart *uart);
// Read one byte from the UART.
uint8_t uart_read_byte(struct uart *uart);
// Return non-zero when a byte can be written.
int uart_write_ready(struct uart *uart);
// Write one byte (blocking until sent).
void uart_write_byte(struct uart *uart, uint8_t byte);
// Write one byte without blocking; returns non-zero on success.
int uart_write_byte_nb(struct uart *uart, uint8_t byte);
// Write a buffer of bytes (blocking).
void uart_write_buf(struct uart *uart, char *buf, size_t len);
// Initialize UART pins and baud rate.
void uart_init(struct uart *uart, uint32_t baud);

// Redirect libc write calls to UART1.
int _write(int fd, char *ptr, int len);
