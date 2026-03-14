#ifndef _uHAL_SYSTICK_
#define _uHAL_SYSTICK_

#include <stdint.h>
#include <concepts>
#include <utility>
#include <functional>

#include "common.hpp"
#include "cortex_m4.h"

namespace uHAL{
    
struct systick{
    using tick_t = uint32_t; // max 24 bits: ticks <= 0x00FFFFFF
    using callback_t = void(*)(void);

    tick_t reload;
    enum class CLKSOURCE{AHB_8, AHB} clock_source;
    enum class TICKINT{IRQ_DISABLE, IRQ_ENABLE} irq;
    enum class ENABLE{SYSTICK_DISABLE, SYSTICK_ENABLE} enable;
    callback_t callback;

    systick(tick_t, CLKSOURCE, TICKINT, callback_t);
    
    void init() const;
    
    static systick period_ms(uint32_t, callback_t);
    static void turn_on_off(ENABLE);
    static void set_reload_value(tick_t);
    static void set_callback(callback_t);
    static void set_irq_on_off(TICKINT);
};
}

#endif