#pragma once
#include "helper.h"
#define ADC ((struct adc *)0x50040000)
#define ADC_COMMON_BASE 0x50040300U

// ADC register map.
struct adc
{
    volatile uint32_t ISR, IER, CR, CFGR, CFGR2,
        SMPR1, SMPR2, RESERVED1, TR1, TR2, TR3, RESERVED2, SQR1, SQR2,
        SQR3, SQR4, DR, RESERVED3[2], JSQR, RESERVED4[4], OFR[4], RESERVED5[4], JDR[4], RESERVED6[4], AWD2CR,
        AWD3CR, RESERVED7[2], DIFSEL, CALFACT;
};

// ADC common register map.
struct adc_common
{
    volatile uint32_t CSR, RESERVED, CCR, CDR;
};

/**
 * @brief Start non-blocking ADC initialization.
 * @param pin Analog pin ID (e.g., A0).
 *
 * Call adc_init_async_poll() until it returns true.
 */
void adc_init_async_start(uint16_t pin);
/**
 * @brief Poll non-blocking ADC initialization state.
 * @param now Current tick time (typically milliseconds).
 * @return true when ADC is initialized and ready.
 */
bool adc_init_async_poll(uint32_t now);

/**
 * @brief Start an ADC conversion without blocking.
 */
void adc_start(void);
/**
 * @brief Check whether the current conversion is complete.
 * @return true when a conversion result is ready.
 */
bool adc_ready(void);
/**
 * @brief Read the most recent conversion result.
 * @return 12-bit ADC result.
 */
uint16_t adc_read_result(void);
