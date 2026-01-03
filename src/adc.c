
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

 void adc_init(uint16_t pin) {

    GPIO(PINBANK(pin))->ASCR |= BIT(PINNO(pin));  // connect analog switch for PC5
    
    // Enable ADC clock and regulator
    RCC->AHB2ENR |= BIT(13);               // ADC clock enable

    ADC->CR = 0;                           // Make sure ADC is disabled

    ((struct adc_common *) ADC_COMMON_BASE)->CCR |= BIT(16); // HCLK/1

    ADC->CR |= BIT(28);                    // ADVREGEN = 0b01
    spin(1000);

    // Calibrate
    ADC->CR |= BIT(31);
    while (ADC->CR & BIT(31)) (void) 0;

    // 1 conversion, channel 14 (PC5), sample time long enough
    ADC->SMPR2 = (7U << 12);               // CH14 sample time
    ADC->SQR1 = (14U << 6);                // SQ1 = channel 14, L = 0
	

    // Enable ADC and wait ready
    ADC->ISR |= BIT(0);                    // clear ADRDY
    ADC->CR |= BIT(0);                     // ADEN
    while ((ADC->ISR & BIT(0)) == 0) (void) 0;
}

void adc_init_async_start(uint16_t pin) {
    s_adc_state = ADC_INIT_WAIT_REG;

    GPIO(PINBANK(pin))->ASCR |= BIT(PINNO(pin));  // connect analog switch
    RCC->AHB2ENR |= BIT(13);               // ADC clock enable
    ADC->CR = 0;                           // Make sure ADC is disabled
    ((struct adc_common *) ADC_COMMON_BASE)->CCR |= BIT(16); // HCLK/1
    ADC->CR |= BIT(28);                    // ADVREGEN = 0b01
    s_adc_wait = 0;
}

bool adc_init_async_poll(uint32_t now) {
    switch (s_adc_state) {
        case ADC_INIT_IDLE:
            return false;
        case ADC_INIT_WAIT_REG:
            if (!timer_expired(&s_adc_wait, 1, now)) return false; // ~1ms
            ADC->CR |= BIT(31);            // start calibration
            s_adc_state = ADC_INIT_WAIT_CAL;
            return false;
        case ADC_INIT_WAIT_CAL:
            if (ADC->CR & BIT(31)) return false;
            ADC->SMPR2 = (7U << 12);       // CH14 sample time
            ADC->SQR1 = (14U << 6);        // SQ1 = channel 14, L = 0
            ADC->ISR |= BIT(0);            // clear ADRDY
            ADC->CR |= BIT(0);             // ADEN
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

 uint16_t adc_read(void) {
    ADC->CR |= BIT(2);                     // ADSTART
    while ((ADC->ISR & BIT(2)) == 0) (void) 0; // wait EOC
    return (uint16_t) ADC->DR;
}


 uint16_t adc_read_avg(uint8_t samples) {
    if (samples == 0) return 0;

    uint32_t acc = 0;
    for (uint8_t i = 0; i < samples; i++) {
        acc += adc_read();
    }
    return (uint16_t)(acc / samples);
}

void adc_start(void) {
    ADC->CR |= BIT(2);                     // ADSTART
}

bool adc_ready(void) {
    return (ADC->ISR & BIT(2)) != 0;
}

uint16_t adc_read_result(void) {
    return (uint16_t) ADC->DR;
}
