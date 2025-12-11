#include "rcc.h"
#include "gpio.h"
#include "helper.h"

#define ADC ((struct adc* ) 0x50040000)
#define ADC_COMMON_BASE 0x50040300U

struct adc{
    volatile uint32_t ISR, IER, CR, CFGR, CFGR2,
    SMPR1, SMPR2, RESERVED1, TR1, TR2, TR3, RESERVED2, SQR1, SQR2,
    SQR3, SQR4, DR, RESERVED3[2], JSQR,RESERVED4[4], OFR[4],RESERVED5[4], JDR[4], RESERVED6[4], AWD2CR,
    AWD3CR, RESERVED7[2], DIFSEL, CALFACT;
};

struct adc_common {
    volatile uint32_t CSR, RESERVED, CCR, CDR;
};

static void adc_init_pc5(void) {
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

static uint16_t adc_read_pc5(void) {
    ADC->CR |= BIT(2);                     // ADSTART
    while ((ADC->ISR & BIT(2)) == 0) (void) 0; // wait EOC
    return (uint16_t) ADC->DR;
}
