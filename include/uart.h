#pragma once
#include "helper.h"
#define UART1 ((struct uart *)0x40013800)
#define UART2 ((struct uart *)0x40004400)
#define UART3 ((struct uart *)0x40004800)
#define UART4 ((struct uart *)0x40004C00)
#define UART5 ((struct uart *)0x40005000)

#define FREQ 4000000

// UART register map.
struct uart
{
    volatile uint32_t CR1, CR2, CR3, BRR, GTPR, GTOR, RQR, ISR, ICR, RDR, TDR;
};

/**
 * @brief Check whether a byte can be written.
 * @param uart UART instance.
 * @return Non-zero if TX can accept a byte.
 */
int uart_write_ready(struct uart *uart);
/**
 * @brief Write one byte (blocking).
 * @param uart UART instance.
 * @param byte Byte to transmit.
 */
void uart_write_byte(struct uart *uart, uint8_t byte);
/**
 * @brief Write one byte without blocking.
 * @param uart UART instance.
 * @param byte Byte to transmit.
 * @return Non-zero on success, zero if not ready.
 */
int uart_write_byte_nb(struct uart *uart, uint8_t byte);
/**
 * @brief Write a buffer of bytes (blocking).
 * @param uart UART instance.
 * @param buf Data buffer.
 * @param len Number of bytes to write.
 */
void uart_write_buf(struct uart *uart, char *buf, size_t len);
/**
 * @brief Initialize UART pins and baud rate.
 * @param uart UART instance.
 * @param baud Baud rate (e.g., 115200).
 */
void uart_init(struct uart *uart, uint32_t baud);

/**
 * @brief Redirect libc write calls to UART1.
 * @param fd File descriptor.
 * @param ptr Buffer to write.
 * @param len Number of bytes.
 * @return -1 on failure, otherwise number of bytes written if supported.
 */
int _write(int fd, char *ptr, int len);
