#ifndef _uHAL_SYSTICK_
#define _uHAL_SYSTICK_

#include <stdint.h>
#include <concepts>
#include <utility>
#include <functional>

#include "cortex_m4.h"
#include "utils.hpp"

namespace uHAL{
    
struct systick{
    using tick_t = uint32_t; // only 24 bits: <= 0x00FFFFFF
    using callback_t = void(*)(void);
    static constexpr tick_t RELOAD_MAX = 0x00FFFFFF;
    
    tick_t _reload;
    enum class CLKSOURCE{AHB_8, AHB} _clock_source;
    enum class TICKINT{IRQ_DISABLE, IRQ_ENABLE} _irq;
    enum class ENABLE{SYSTICK_DISABLE, SYSTICK_ENABLE} _enabled;
    // todo NOREF & SKEW
    static constexpr uint32_t _TENMS = 10500; // (#tick/s) see ref manual for info
    callback_t _callback;
    
    void init() const;

    static systick period_ms(uint32_t, callback_t);
};
}

#endif