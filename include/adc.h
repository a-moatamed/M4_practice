#pragma once
#include "helper.h"

#define ADC ((struct adc *)0x50040000)
#define ADC_COMMON_BASE 0x50040300U

struct adc
{
    volatile uint32_t ISR, IER, CR, CFGR, CFGR2,
        SMPR1, SMPR2, RESERVED1, TR1, TR2, TR3, RESERVED2, SQR1, SQR2,
        SQR3, SQR4, DR, RESERVED3[2], JSQR, RESERVED4[4], OFR[4], RESERVED5[4], JDR[4], RESERVED6[4], AWD2CR,
        AWD3CR, RESERVED7[2], DIFSEL, CALFACT;
};

struct adc_common
{
    volatile uint32_t CSR, RESERVED, CCR, CDR;
};

void adc_init(uint16_t pin);
void adc_init_async_start(uint16_t pin);
bool adc_init_async_poll(uint32_t now);

uint16_t adc_read(void);
uint16_t adc_read_avg(uint8_t samples);
void adc_start(void);
bool adc_ready(void);
uint16_t adc_read_result(void);
