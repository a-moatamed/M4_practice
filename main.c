#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gpio.h"
#include "helper.h"
#include "rcc.h"
#include "systick.h"
#include "timer.h"
#include "adc.h"
#include "uart.h"

#include <stdio.h>

#define CLOCK 4000000

volatile uint32_t s_ticks = 0;

int main(void)
{
    uart_init(UART1, 115200);  // set up PB6/PB7 as TX/RX and enable UART1

    uint16_t pot = PIN('C', 5);
    gpio_set_mode(pot, GPIO_MODE_ANALOG);
    GPIO(PINBANK(pot))->ASCR |= BIT(PINNO(pot));  // Close analog switch for PC5

    adc_init_pc5();

    volatile uint16_t pot_raw = 0;

    while (1)
    {
        pot_raw = adc_read_pc5_avg(16); // average 16 samples to smooth noise

        
        printf("READ VALUE: %d\r\n", pot_raw);
        spin(200000); // simple throttle to avoid flooding the UART
    }
}
