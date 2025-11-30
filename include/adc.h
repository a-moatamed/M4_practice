// #include "rcc.h"
// #include "gpio.h"

// #define ADC ((struct adc* ) 0x50040000)
// #define ADC_COMMON_BASE 0x50040300UL


// //#####################################################################

// // ADC_CR bits
// #define ADC_CR_DEEPPWD       (1 << 29)
// #define ADC_CR_ADVREGEN      (1 << 28)
// #define ADC_CR_ADCAL         (1u << 31)
// #define ADC_CR_ADEN          (1 << 0)
// #define ADC_CR_ADSTART       (1 << 2)

// // ADC_ISR bits
// #define ADC_ISR_ADRDY        (1 << 0)
// #define ADC_ISR_EOC          (1 << 2)

// // ADC_CFGR bits
// #define ADC_CFGR_CONT        (1 << 13)

// // ################################################################################

// // for simplicity purpose
// #define chanels 1

// struct adc{

// 	volatile uint32_t ISR, IER, CR, CFGR, CFGR2,
// 	SMPR[2], TR1, TR2, TR3, RESERVED2, SQR1, SQR2,
// 	SQR3, SQR4, DR, JSQR, OFR[4], JDR[4], AWD2CR,
// 	AWD3CR, DIFSEL, CALFACT;
// };

// struct adc_common {
//     volatile uint32_t CSR, CCR, CDR;
// };


// // set the ADC1 and PA0 pin for now and only one chanel
// void adc_init(struct adc* adc = ADC, uint8_t pin = PIN('A', 0), uint8_t chanel)
// {

// 	//enable GPIO port for the pin
// 	RCC->AHB2ENR |= BIT(PINBANK(pin));
// 	gpio_set_mode(pin, GPIO_MODE_ANALOG);

// 	// RCC_AHB2 ENR.  -- pin13 // chanel 5
// 	RCC->AHB2ENR |= BIT(13);

// 	// ################################################################################
// 	adc->CR &= ~ADC_CR_DEEPPWD;
// 	adc->CR |= ADC_CR_ADVREGEN;

// 	for (volatile int i = 0; i < 2000; i++);

// 	adc->CR |= ADC_CR_ADCAL;
// 	while (adc->CR & ADC_CR_ADCAL);

// 	adc->ISR |= ADC_ISR_ADRDY;  // clear ADRDY
//     adc->CR  |= ADC_CR_ADEN;

// 	// wait until ADRDY = 1
//     while (!(adc->ISR & ADC_ISR_ADRDY));

//     // 7. Configure regular sequence: 1 conversion, channel 11
//     // SQR1[ L=0, Rank1=11 ]
//     adc->SQR1 = (11u << 6);

//     // 8. Enable continuous conversion mode
//     adc->CFGR |= ADC_CFGR_CONT;

//     // 9. Start conversions
//     adc->CR |= ADC_CR_ADSTART;


// 	// ################################################################################
	
// 	// //ADC_CR: enable voltage regulattor
// 	// 	// start calibration
// 	// 	// enable ADC
// 	// 	// start conversion.
// 	// adc->CR |= BIT(0) | BIT(2) |BIT(28) | BIT(31);

// 	// //enable cotinuse mode and set the resolution and alignment. keep 0 in resolution to be
// 	// // 12-bit and 0 in alignment to be right alignment.
// 	// adc->CFGR |= BIT(13);

// 	// // set the number of chanels
// 	// adc->SQR1 &= ~(7UL << 0);
// 	// adc->SQR1 |= chanel;

// 	// // set the order of the conversion
// 	// adc->SQR1 &= ~(7UL << 6 * (chanels));
// 	// adc->SQR1 |= chanel;

// }

// // // sampling cycles
// // enum {SMP_2_5, SMP_6_5, SMP_12_5, SMP_24_5, SMP_47_5, SMP_92_5, SMP_247_5, SMP_640_5};

// // void set_sampling_time(struct adc* adc, uint8_t cycle_numbers, uint8_t chanel){
// // // 	gpio->AFR[n >> 3] &= ~(15UL << ((n & 7) * 4));
// // //   gpio->AFR[n >> 3] |= ((uint32_t) af_num) << ((n & 7) * 4);

// // // set sampilng cycles numbber.
// // adc->SMPR[chanel / 9] &= ~ (7UL << (chanel * 3));
// // adc->SMPR[chanel / 9] |= ((uint32_t) chanel) << (chanel * 3);
// // }

// // uint32_t read_adc (struct adc* adc){
// // 	return 0xffff & adc->DR;
// // }


// uint32_t ADC1_Read(void)
// {
//     while (!(adc->ISR & ADC_ISR_EOC));   // wait end-of-conversion
//     return ((uint32_t) adc->DR);      // reading clears EOC
// }





#include "rcc.h"
#include "gpio.h"

// ADC1 base
#define ADC ((struct adc* ) 0x50040000)

//#####################################################################

// ADC_CR bits
#define ADC_CR_DEEPPWD       (1 << 29)
#define ADC_CR_ADVREGEN      (1 << 28)
#define ADC_CR_ADCAL         (1u << 31)
#define ADC_CR_ADEN          (1 << 0)
#define ADC_CR_ADSTART       (1 << 2)

// ADC_ISR bits
#define ADC_ISR_ADRDY        (1 << 0)
#define ADC_ISR_EOC          (1 << 2)

// ADC_CFGR bits
#define ADC_CFGR_CONT        (1 << 13)


struct adc {
	volatile uint32_t ISR, IER, CR, CFGR, CFGR2,
	SMPR[2], TR1, TR2, TR3, RESERVED2, SQR1, SQR2,
	SQR3, SQR4, DR, JSQR, OFR[4], JDR[4], AWD2CR,
	AWD3CR, DIFSEL, CALFACT;
};

// ---------------------------------------------------------------------------
//  ADC INITIALIZATION (your style, your structs, corrected)
// ---------------------------------------------------------------------------
void adc_init(struct adc* adc, uint8_t pin, uint8_t chanel)
{
    // 1. Enable GPIO clock for this pin
    RCC->AHB2ENR |= BIT(PINBANK(pin));

    // 2. Set pin to analog mode
    gpio_set_mode(pin, GPIO_MODE_ANALOG);

    // 3. Enable ADC1 clock
    RCC->AHB2ENR |= BIT(13);   // ADCEN

    // 4. Exit deep-power-down mode
    adc->CR &= ~ADC_CR_DEEPPWD;

    // 5. Enable ADC regulator
    adc->CR |= ADC_CR_ADVREGEN;

    // small stabilization delay
    for (volatile int i = 0; i < 2000; i++);

    // 6. Start ADC calibration
    adc->CR |= ADC_CR_ADCAL;
    while (adc->CR & ADC_CR_ADCAL);

    // 7. Enable ADC
    adc->ISR |= ADC_ISR_ADRDY;     // clear ADRDY
    adc->CR  |= ADC_CR_ADEN;

    while (!(adc->ISR & ADC_ISR_ADRDY));

    // 8. Configure channel sequence
    // Only ONE conversion (L = 0)
    adc->SQR1 = 0;
    adc->SQR1 |= ((uint32_t)chanel << 6);

    // 9. Continuous mode
    adc->CFGR |= ADC_CFGR_CONT;

    // 10. Start conversion
    adc->CR |= ADC_CR_ADSTART;
}


// ---------------------------------------------------------------------------
//  ADC READ (your style, corrected)
// ---------------------------------------------------------------------------
uint32_t ADC1_Read(struct adc* adc)
{
    while (!(adc->ISR & ADC_ISR_EOC));  // wait for end-of-conversion
    return adc->DR;                     // read result
}
