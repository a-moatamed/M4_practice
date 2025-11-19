
extern int main(void);

__attribute__((naked, noreturn)) void _reset(void) {
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;
    main();
    for (;;) (void)0;
}

extern void _reset(void);
extern unsigned long _estack;

extern void SysTick_Handler(void);

__attribute__((section(".vectors"))) 
void (*const tab[16 + 91])(void) = {
    (void (*)(void))(&_estack), _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler};
