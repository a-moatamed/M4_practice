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

int uart_read_ready(struct uart *uart);
uint8_t uart_read_byte(struct uart *uart);
void uart_write_byte(struct uart *uart, uint8_t byte);
void uart_write_buf(struct uart *uart, char *buf, size_t len);
void uart_init(struct uart *uart, uint32_t baud);

// this function to redirct the write function in uart to printf which is more cabable and easy to expose more info
int _write(int fd, char *ptr, int len);
