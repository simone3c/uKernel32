#include "uHAL/systick.hpp"

using uHAL::systick;

systick::systick(tick_t reload, CLKSOURCE clock_source, TICKINT irq, ENABLE en):
    _reload(reload & RELOAD_MAX),
    _clock_source(clock_source),
    _irq(irq),
    _enabled(en)
{}

// todo remove hardcoded values
void systick::configure(){
    set_property(_reload);
    SYSTICK->VAL = 0;
    set_property(_clock_source);
    set_property(_irq);
    SYSTICK->CALIB |= _TENMS & (BIT(24) - 1);
    // this must be last
    set_property(_enabled);
}

systick systick::period_ms(uint32_t ms){
    uint64_t reload = ms * 16000000 / 1000; // todo

    return systick{
        static_cast<uint32_t>(reload) & RELOAD_MAX, 
        CLKSOURCE::AHB_8, 
        TICKINT::IRQ_ENABLED,
        ENABLE::SYSTICK_ENABLED
    };
}

__attribute__((weak)) void uHAL::systick::callback(void){
    // EMPTY
}

void systick_irq_handler(void){
    uHAL::systick::callback();
}
