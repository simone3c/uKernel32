#include <functional>

#include "uHAL/systick.hpp"

using uHAL::systick;

static uHAL::systick::callback_t _systick_callback = []() {
    // empty
    (void)0;
};

template<typename T>
    requires
    std::same_as<T, systick::CLKSOURCE> ||
    std::same_as<T, systick::TICKINT> ||
    std::same_as<T, systick::ENABLE>
void set_property_enum(T prop) {
    
    uint8_t mask_len = 0; // std::same_as<T, systick::ENABLE>
    if constexpr (std::same_as<T, systick::TICKINT>) {
        mask_len = 1;
    }
    else if constexpr (std::same_as<T, systick::CLKSOURCE>) {
        mask_len = 2;
    }

    const uint32_t data = std::to_underlying(prop) << mask_len;
    uHAL::LL_write_reg(SYSTICK->CTRL, data, uHAL::BIT(mask_len));
}

// todo remove hardcoded values
void systick::init() const {
    _systick_callback = _callback;

    LL_write_reg(SYSTICK->LOAD, _reload, systick::RELOAD_MAX);

    LL_write_reg(SYSTICK->VAL, 0);

    set_property_enum(_clock_source);
    set_property_enum(_irq);
    
    LL_write_reg(SYSTICK->CALIB, _TENMS, BIT(24) - 1);

    // this must be last
    set_property_enum(_enabled);
}

systick systick::period_ms(uint32_t ms, callback_t cb){
    uint32_t reload = ms * 16000; // todo cpu clock of 16MHz by default

    return systick{
        static_cast<uint32_t>(reload) & RELOAD_MAX, 
        CLKSOURCE::AHB,
        TICKINT::IRQ_ENABLE,
        ENABLE::SYSTICK_ENABLE,
        cb
    };
}

void systick_irq_handler(void){
    _systick_callback();
}
