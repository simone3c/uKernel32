__attribute((weak)) void _systick_irq_handler(void){
    // EMPTY
    (void)0;
}

extern "C"{
    
extern void _estack(void);  // Defined in link.ld
extern int main(void); // user-defined


// Startup code
__attribute__((naked, noreturn)) void _reset(void) {
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    
    for (long* dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
    
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

    // todo static constructors?
    // extern void(*__fini_array_start)(void);
    // extern void(*__fini_array_end)(void);
    // long fini_sz = __fini_array_end - __fini_array_start;

    // for(int i = 0; i < fini_sz; ++i){
    //     (*(__fini_array_start + i))();
    // }

    
    main();
    
    for (;;) (void)0;  // Infinite loop - should never be reached
}

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
    _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _systick_irq_handler
};

}