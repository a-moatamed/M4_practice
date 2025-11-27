#include "uart.h"
void uart_init(struct uart *uart, uint32_t baud)
{
    uint16_t tx = 0, rx = 0;
    uint8_t af = (uart == UART4) ? 8 : 7;   // Default AF for most UARTs (including USART1)

    // Enable UART clock
    if (uart == UART1) {
        RCC->APB2ENR |= BIT(14);
        // tx = PIN('B', 6);
        // rx = PIN('B', 7);

        tx = PIN('A', 2); // tx, rx according tot 
        rx = PIN('A', 3);

        // tx = PIN('A', 9); // tx, rx according tot 
        // rx = PIN('A', 10);
        
        
    }
    else if (uart == UART2) {
        RCC->APB1ENR1 |= BIT(17);
        // TODO: Set UART2 pins later
    }
    else if (uart == UART3) {
        RCC->APB1ENR1 |= BIT(18);
        // TODO: Set UART3 pins later
    }
    else if (uart == UART4) {
        RCC->APB1ENR1 |= BIT(19);
        // TODO: UART4 pins (AF8)
        
    }
    else if (uart == UART5) {
        RCC->APB1ENR1 |= BIT(20);
        // TODO: UART5 pins
    }

    // Only configure pins if valid pin values exist
    if (tx && rx) {
        gpio_set_mode(tx, GPIO_MODE_AF);
        gpio_set_af(tx, af);

        gpio_set_mode(rx, GPIO_MODE_AF);
        gpio_set_af(rx, af);
    }

    // Configure UART
    uart->CR1 = 0;                          
    uart->BRR = FREQ / (2 * 8 * baud);                
    uart->CR1 |= BIT(13) | BIT(2) | BIT(3); // UE, RE, TE
}





