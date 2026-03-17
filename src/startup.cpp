using func_ptr = void(*)(void);

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

    extern func_ptr __preinit_array_start[0];
    extern func_ptr __preinit_array_end[0];
    for(func_ptr* func = __preinit_array_start; func != __preinit_array_end; ++func){
        (*func)();
    }

    extern func_ptr __init_array_start[0];
    extern func_ptr __init_array_end[0];
    for(func_ptr* func = __init_array_start; func != __init_array_end; ++func){
        (*func)();
    }

    main();
    
    extern func_ptr __fini_array_start[0];
    extern func_ptr __fini_array_end[0];
    for(func_ptr* func = __fini_array_start; func != __fini_array_end; ++func){
        (*func)();
    }
    
    for (;;) (void)0;  // Infinite loop - should never be reached
}

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
    _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _systick_irq_handler
};

}