#include "uart.h"

void uart_write_byte(struct uart *uart, uint8_t byte)
{
    uart->TDR = byte;
    while ((uart->ISR & BIT(7)) == 0)
        spin(1);
}

int uart_write_ready(struct uart *uart)
{
    return (uart->ISR & BIT(7)) != 0;
}

int uart_write_byte_nb(struct uart *uart, uint8_t byte)
{
    if (!uart_write_ready(uart)) return 0;
    uart->TDR = byte;
    return 1;
}

void uart_write_buf(struct uart *uart, char *buf, size_t len)
{
    while (len-- > 0)
        uart_write_byte(uart, *(uint8_t *)buf++);
}

void uart_init(struct uart *uart, uint32_t baud)
{
    uint16_t tx = 0, rx = 0;
    uint8_t af = (uart == UART4) ? 8 : 7;

    if (uart == UART1) {
        RCC->APB2ENR |= BIT(14);
        tx = PIN('B', 6);
        rx = PIN('B', 7);
    }
    else if (uart == UART2) {
        RCC->APB1ENR1 |= BIT(17);
    }
    else if (uart == UART3) {
        RCC->APB1ENR1 |= BIT(18);
    }
    else if (uart == UART4) {
        RCC->APB1ENR1 |= BIT(19);
    }
    else if (uart == UART5) {
        RCC->APB1ENR1 |= BIT(20);
    }

    gpio_set_mode(tx, GPIO_MODE_AF);
    gpio_set_af(tx, af);

    gpio_set_mode(rx, GPIO_MODE_AF);
    gpio_set_af(rx, af);

    uart->CR1 = 0;
    uart->BRR = FREQ / baud;
    uart->CR1 |= BIT(0) | BIT(2) | BIT(3);
}


int _write(int fd, char *ptr, int len) {
  (void) fd, (void) ptr, (void) len;
  if (fd == 1) uart_write_buf(UART1, ptr, (size_t) len);
  return -1;
}
