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



struct systick{
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
};
#define SYSTICK ((struct systick *) 0xe000e010)




static inline void systick_init(uint32_t ticks){
    if ((ticks - 1) > 0xffffff) return; // maximum value is 24 bit (SYSTICK is 24 bit)
    SYSTICK -> LOAD = ticks - 1;
    SYSTICK -> VAL = 0;
    SYSTICK -> CTRL = BIT(0) | BIT(1) | BIT(2); // enable systick
    RCC -> APB2ENR |= BIT(0); // enable syscfg
}


static volatile uint32_t s_ticks; // volatile is important!!
void SysTick_Handler(void) {
  s_ticks++;
}



// t: expiration time, prd: period, now: current time. Return true if expired
bool timer_expired(uint32_t *t, uint32_t prd, uint32_t now) {
  if (now + prd < *t) *t = 0;                    // Time wrapped? Reset timer
  if (*t == 0) *t = now + prd;                   // First poll? Set expiration
  if (*t > now) return false;                    // Not expired yet, return
  *t = (now - *t) > prd ? now + prd : *t + prd;  // Next expiration time
  return true;                                   // Expired, return true
}



int main(void) {
    uint16_t led = PIN('B', 14);
    uint16_t led2 = PIN('B', 2);
    RCC->AHB2ENR |= BIT(PINBANK(led)); // Enable GPIOB clock
    // RCC->AHB2ENR |= BIT(PINBANK(led2)); // Enable GPIOB clock
    gpio_set_mode(led, GPIO_MODE_OUTPUT);
    gpio_set_mode(led2, GPIO_MODE_OUTPUT);
    systick_init(4000000/1000);
    uint32_t timer = 0, period = 1000;
    for (;;) {
        if(timer_expired(&timer, period, s_ticks)){
            static bool light;
            static bool light2;
            gpio_write(led, light);
            gpio_write(led2, light2);
            light =! light;
            light2 =! light2;

        }
        
    }
}

__attribute__((naked, noreturn)) void _reset(void) {
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;
    main();
    for (;;) (void)0;
}

extern void _reset(void);
extern unsigned long _estack;
__attribute__((section(".vectors"))) 
void (*const tab[16 + 91])(void) = {
    (void (*)(void))(&_estack), _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler};