#include <inttypes.h>
#include <stdbool.h>

#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

struct gpio {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
};

#define GPIO(bank) ((struct gpio *) (0x48000000 + 0x400 * (bank)))  // ✅ STM32L4 GPIO base

struct rcc {
    volatile uint32_t CR, ICSCR, CFGR, PLLCFGR, PLLSAI1CFGR, PLLSAI2CFGR;
    volatile uint32_t CIER, CIFR, CICR, RESERVED0;
    volatile uint32_t AHB1RSTR, AHB2RSTR, AHB3RSTR, RESERVED1;
    volatile uint32_t APB1RSTR1, APB1RSTR2, APB2RSTR, RESERVED2;
    volatile uint32_t AHB1ENR, AHB2ENR, AHB3ENR, RESERVED3;
    volatile uint32_t APB1ENR1, APB1ENR2, APB2ENR, RESERVED4;
    volatile uint32_t AHB1SMENR, AHB2SMENR, AHB3SMENR, RESERVED5;
    volatile uint32_t APB1SMENR1, APB1SMENR2, APB2SMENR, RESERVED6;
    volatile uint32_t CCIPR, RESERVED7, BDCR, CSR, CRRCR, CCIPR2;
};
#define RCC ((struct rcc *)0x40021000)

enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
    struct gpio *gpio = GPIO(PINBANK(pin));
    int n = PINNO(pin);
    gpio->MODER &= ~(3U << (n * 2));
    gpio->MODER |= (mode & 3) << (n * 2);
}

static inline void gpio_write(uint16_t pin, bool val) {
    struct gpio *gpio = GPIO(PINBANK(pin));
    gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
}

static inline void spin(volatile uint32_t count) {
    while (count--) (void)0;
}

extern void _reset(void);
extern unsigned long _estack;

__attribute__((section(".vectors")))
void (*const vector_table[])(void) = {
    (void (*)(void))(&_estack),
    _reset
};

int main(void) {
    uint16_t led = PIN('B', 14);
    RCC->AHB2ENR |= BIT(PINBANK(led)); // Enable GPIOB clock
    gpio_set_mode(led, GPIO_MODE_OUTPUT);

    for (;;) {
        gpio_write(led, true);
        spin(500000);
        gpio_write(led, false);
        spin(500000);
    }
}

__attribute__((naked, noreturn)) void _reset(void) {
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;
    main();
    for (;;) (void)0;
}
