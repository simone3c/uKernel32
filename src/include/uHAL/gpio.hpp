#ifndef _uHAL_GPIO_
#define _uHAL_GPIO_

#include <cstdint>
#include <concepts>
#include <utility>

#include "error.hpp"
#include "common.hpp"
#include "stm32f401xe.h"

namespace uHAL{

    struct GPIO{

        using pin_bitmask_t = uint16_t;
        
        template <unsigned n>
        requires (n < 16)
        static constexpr pin_bitmask_t PIN = BIT(n);

        pin_bitmask_t _pins;
        enum class PORT{A, B, C, D, E, H} _port;
        enum class MODE{INPUT, OUTPUT, ALTERNATE, ANALOG} _mode;
        enum class OTYPE{PUSH_PULL, OPEN_DRAIN} _otype;
        enum class OSPEED{LOW, MEDIUM, HIGH, VERY_HIGH} _ospeed;
        enum class PUPD{NONE, PULL_UP, PULL_DOWN} _pupd;
        
        // TODO alternate func e lock e irq

        void init() const;
        void set_level(bool) const;
        void toggle_level() const;

        // --- static methods ---

        static GPIO create_output(
            PORT,
            pin_bitmask_t, 
            OTYPE = OTYPE::PUSH_PULL, 
            OSPEED = OSPEED::LOW
        );

        static void set_level(PORT, pin_bitmask_t, bool);
        static void toggle_level(PORT, pin_bitmask_t);

    };
}

#endif