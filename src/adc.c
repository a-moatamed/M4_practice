
 #include "adc.h"
 

 
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
