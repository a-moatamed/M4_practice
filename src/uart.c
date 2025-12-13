#include "uart.h"


// function to check it the data is ready to read 
int uart_read_ready(struct uart *uart)
{
    return uart->ISR & BIT(5); // If RXNE bit is set, data is ready
}



// function to read a byte if it's ready
uint8_t uart_read_byte(struct uart *uart)
{
    return (uint8_t)(uart->RDR & 255);
}


// function to write a byte if it's ready
void uart_write_byte(struct uart *uart, uint8_t byte)
{
    uart->TDR = byte;
    while ((uart->ISR & BIT(7)) == 0)
        spin(1);
}


void uart_write_buf(struct uart *uart, char *buf, size_t len)
{
    while (len-- > 0)
        uart_write_byte(uart, *(uint8_t *)buf++);
}


void uart_init(struct uart *uart, uint32_t baud)
{
    uint16_t tx = 0, rx = 0;
    uint8_t af = (uart == UART4) ? 8 : 7;   // Default AF for most UARTs (including USART1) is 7

    // Enable UART clock
    if (uart == UART1) {
        RCC->APB2ENR |= BIT(14);
        tx = PIN('B', 6);
        rx = PIN('B', 7);

        
    }
    else if (uart == UART2) {
        RCC->APB1ENR1 |= BIT(17);
        // TODO: Set UART2 pins later
    }
    else if (uart == UART3) {
        RCC->APB1ENR1 |= BIT(18);
        // TODO: Set UART3 pins later.
    }
    else if (uart == UART4) {
        RCC->APB1ENR1 |= BIT(19);
        // TODO: UART4 pins (AF8)
        
    }
    else if (uart == UART5) {
        RCC->APB1ENR1 |= BIT(20);
        // TODO: UART5 pins
    }

 
    gpio_set_mode(tx, GPIO_MODE_AF);
    gpio_set_af(tx, af);

    gpio_set_mode(rx, GPIO_MODE_AF);
    gpio_set_af(rx, af);


    // Configure UART
    uart->CR1 = 0;                          
    uart->BRR = FREQ / baud;                
    uart->CR1 |= BIT(0) | BIT(2) | BIT(3); // set UE, RE, TE
}


int _write(int fd, char *ptr, int len) {
  (void) fd, (void) ptr, (void) len;
  if (fd == 1) uart_write_buf(UART1, ptr, (size_t) len);
  return -1;
}


