#include <utility>

#include "uHAL/systick.hpp"

#include "uHAL/common.hpp"
#include "cortex_m4.h"

using uHAL::systick;
using uHAL::LL_register;

template<typename T>
concept ctrl_property = 
    std::same_as<T, systick::clksource_t> ||
    std::same_as<T, systick::tickint_t> ||
    std::same_as<T, systick::status_t>;
    
static constexpr systick::tick_t RELOAD_MAX = 0x00FFFFFF; // max 24 bits

static uHAL::systick::callback_t _systick_callback = nullptr;

void _systick_irq_handler(void){
    _systick_callback();
}

// ---- specific registers abstraction ----

struct ctrl_reg{
    static constexpr LL_register<SYSTICK> reg{};

    template<ctrl_property T>
    static void set_property(T prop) {
    
        uint8_t mask_len = 0; // std::same_as<T, systick::ENABLE>
        if constexpr (std::same_as<T, systick::tickint_t>) {
            mask_len = 1;
        }
        if constexpr (std::same_as<T, systick::clksource_t>) {
            mask_len = 2;
        }

        const uint32_t data = std::to_underlying(prop) << mask_len;
        reg.set(data, uHAL::BIT(mask_len));
    }
};

struct load_reg{
    static constexpr LL_register<SYSTICK + 4> reg{};

    static inline void set_reload(systick::tick_t reload) {
        reg.set(RELOAD_MAX, reload);
    }
};

struct val_reg{
    static constexpr LL_register<SYSTICK + 8> reg{};

    static inline void set_val(systick::tick_t value) {
        reg.set(RELOAD_MAX, value);
    }
};

// ---- methods implementation ----

void systick::apply() const {
    set_callback(callback);

    load_reg::set_reload(reload);

    val_reg::set_val(0);

    ctrl_reg::set_property(clock_source);
    ctrl_reg::set_property(irq);
    // this must be last
    ctrl_reg::set_property(status_t::enabled);
}

systick systick::period_ms(uint32_t ms, callback_t cb){
    uint32_t reload = ms * 16000; // todo cpu clock of 16MHz by default

    return systick{
        .reload = static_cast<uint32_t>(reload) & RELOAD_MAX, 
        .clock_source = clksource_t::ahb,
        .irq = tickint_t::irq_enabled,
        .callback = cb
    };
}

void systick::turn_on_off(status_t status){
    ctrl_reg::set_property(status);
}

void systick::set_reload_value(tick_t ticks){
    load_reg::set_reload(ticks);
}

void systick::set_callback(callback_t cb){
    _systick_callback = cb;
}

void systick::set_irq_on_off(tickint_t en){
    ctrl_reg::set_property(en);
}

