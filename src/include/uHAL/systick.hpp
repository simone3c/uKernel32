#ifndef _uHAL_SYSTICK_
#define _uHAL_SYSTICK_

#include <stdint.h>
#include <concepts>
#include <utility>
#include <functional>

#include "common.hpp"
#include "cortex_m4.h"

namespace uHAL{
    
class systick{
public:
    using tick_t = uint32_t; // max 24 bits: <= 0x00FFFFFF
    using callback_t = void(*)(void);
    enum class CLKSOURCE{AHB_8, AHB};
    enum class TICKINT{IRQ_DISABLE, IRQ_ENABLE};
    enum class ENABLE{SYSTICK_DISABLE, SYSTICK_ENABLE};

private:
    tick_t _reload;
    CLKSOURCE _clock_source;
    TICKINT _irq;
    ENABLE _enabled;
    // todo NOREF & SKEW
    static constexpr uint32_t _TENMS = 10500; // (#tick/s) see ref manual for info
    callback_t _callback;

public:
    void init() const;
    systick(tick_t, CLKSOURCE, TICKINT, ENABLE, callback_t);
    static systick period_ms(uint32_t, callback_t);
    // todo add fun to set single features (?)
};
}

#endif