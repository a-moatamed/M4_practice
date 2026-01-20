#include "adc.h"

enum adc_init_state {
    ADC_INIT_IDLE = 0,
    ADC_INIT_WAIT_REG,
    ADC_INIT_WAIT_CAL,
    ADC_INIT_WAIT_RDY,
    ADC_INIT_DONE
};

static enum adc_init_state s_adc_state = ADC_INIT_IDLE;
static uint32_t s_adc_wait = 0;


void adc_init_async_start(uint16_t pin) {
    s_adc_state = ADC_INIT_WAIT_REG;

    GPIO(PINBANK(pin))->ASCR |= BIT(PINNO(pin));
    RCC->AHB2ENR |= BIT(13);
    ADC->CR = 0;
    ((struct adc_common *) ADC_COMMON_BASE)->CCR |= BIT(16);
    ADC->CR |= BIT(28);
    s_adc_wait = 0;
}

bool adc_init_async_poll(uint32_t now) {
    switch (s_adc_state) {
        case ADC_INIT_IDLE:
            return false;
        case ADC_INIT_WAIT_REG:
            if (!timer_expired(&s_adc_wait, 1, now)) return false;
            ADC->CR |= BIT(31);
            s_adc_state = ADC_INIT_WAIT_CAL;
            return false;
        case ADC_INIT_WAIT_CAL:
            if (ADC->CR & BIT(31)) return false;
            ADC->SMPR2 = (7U << 12);
            ADC->SQR1 = (14U << 6);
            ADC->ISR |= BIT(0);
            ADC->CR |= BIT(0);
            s_adc_state = ADC_INIT_WAIT_RDY;
            return false;
        case ADC_INIT_WAIT_RDY:
            if ((ADC->ISR & BIT(0)) == 0) return false;
            s_adc_state = ADC_INIT_DONE;
            return true;
        case ADC_INIT_DONE:
            return true;
        default:
            return false;
    }
}


void adc_start(void) {
    ADC->CR |= BIT(2);
}

bool adc_ready(void) {
    return (ADC->ISR & BIT(2)) != 0;
}

uint16_t adc_read_result(void) {
    return (uint16_t) ADC->DR;
}
