#include "gpio.h"

static inline void spin(volatile uint32_t count) {
    while (count--) (void)0;
}

int main(void) {
    
    uHAL::GPIO led = uHAL::GPIO::output_pin(5, uHAL::GPIO::PORT::A);
    led.configure();
    while(4){
        uHAL::GPIO::set_level(uHAL::GPIO::PORT::A, 5, 1);
        spin(999999);
        
        uHAL::GPIO::set_level(uHAL::GPIO::PORT::A, 5, 0);
        spin(999999);
    }
}




extern "C"{

extern void _estack(void);  // Defined in link.ld


// Startup code
__attribute__((naked, noreturn)) void _reset(void) {
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    
    for (long* dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
    
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;
    
    main();
    
    for (;;) (void)0;  // Infinite loop - should never be reached
}



// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
    _estack, _reset
};
}