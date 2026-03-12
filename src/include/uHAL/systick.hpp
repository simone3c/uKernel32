#ifndef _uHAL_SYSTICK_
#define _uHAL_SYSTICK_

#include <stdint.h>
#include <concepts>
#include <utility>

#include "cortex_m4.h"
#include "utils.hpp"

namespace uHAL{
    
struct systick{
    using tick_t = uint32_t; // only 24 bits: <= 0x00FFFFFF
    static constexpr tick_t RELOAD_MAX = 0x00FFFFFF;
    
    tick_t _reload;
    enum class CLKSOURCE{AHB_8, CPU_CLK} _clock_source;
    enum class TICKINT{IRQ_DISABLE, IRQ_ENABLED} _irq;
    enum class ENABLE{SYSTICK_DISABLED, SYSTICK_ENABLED} _enabled;
    // todo NOREF & SKEW
    static constexpr uint32_t _TENMS = 10500; // (#tick/s) see ref manual for info
    

    systick(tick_t, CLKSOURCE, TICKINT, ENABLE);

    void configure();

    template<typename T>
    requires 
        std::same_as<T, tick_t> || 
        std::same_as<T, CLKSOURCE> || 
        std::same_as<T, TICKINT> || 
        std::same_as<T, ENABLE>
    static void set_property(T);


    static systick period_ms(uint32_t);

    static void callback(void);
 
};

template<typename T>
requires 
    std::same_as<T, systick::tick_t> || 
    std::same_as<T, systick::CLKSOURCE> || 
    std::same_as<T, systick::TICKINT> || 
    std::same_as<T, systick::ENABLE>
void systick::set_property(T prop){
    auto* systick_raw_ptr = SYSTICK;

    if constexpr (std::same_as<T, systick::tick_t>){
        systick_raw_ptr->LOAD &= ~systick::RELOAD_MAX;
        systick_raw_ptr->LOAD |= prop & systick::RELOAD_MAX;
    }
    else{
        int bit;
        if constexpr (std::same_as<T, systick::CLKSOURCE>){
            bit = 2;
        }
        else if constexpr (std::same_as<T, systick::TICKINT>){
            bit = 1;
        }
        else if constexpr (std::same_as<T, systick::ENABLE>){
            bit = 0;
        }

        systick_raw_ptr->CTRL &= ~BIT(bit);
        systick_raw_ptr->CTRL |= std::to_underlying(prop) << bit;

    }
}

}

#endif