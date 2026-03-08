#ifndef _uHAL_GPIO_
#define _uHAL_GPIO_

#include <stdint.h>
#include <concepts>
#include <utility>

#include "error.hpp"
#include "stm32f401xe.h"

#define RETURN_LEFT_IF_DIFFERENT(left, right) if(ret = (left); ret != (right)) { return ret }


namespace uHAL{
    struct GPIO{

        using pin_bitmask_t = uint16_t;
        
        template <unsigned n>
        requires (n < 16)
        static constexpr pin_bitmask_t PIN = 1UL << n;

        pin_bitmask_t _pins;
        enum class PORT{A, B, C, D, E, H} _port;
        enum class MODE{INPUT, OUTPUT, ALTERNATE, ANALOG} _mode;
        enum class OTYPE{PUSH_PULL, OPEN_DRAIN} _otype;
        enum class OSPEED{LOW, MEDIUM, HIGH, VERY_HIGH} _ospeed;
        enum class PUPD{NONE, PULL_UP, PULL_DOWN} _pupd;
        
        // TODO alternate func e lock

        GPIO(PORT, pin_bitmask_t, MODE, OTYPE, OSPEED, PUPD);

        err_t set_gpio_property(auto);

        void configure();

        // --- static methods ---

        static GPIO create_output(
            PORT,
            pin_bitmask_t, 
            OTYPE otype = OTYPE::PUSH_PULL, 
            OSPEED ospeed = OSPEED::LOW
        );

        template<typename T>
        requires 
            std::same_as<T, GPIO::MODE> || 
            std::same_as<T, GPIO::OTYPE> || 
            std::same_as<T, GPIO::OSPEED> || 
            std::same_as<T, GPIO::PUPD>
        static err_t set_property(PORT p, pin_bitmask_t pins, T prop){
            GPIO_TypeDef* port_hw = get_port_ptr(p);
            volatile uint32_t* field_hw = nullptr;

            uint8_t mask = 0x03;
            uint8_t mask_bit_len = 2;

            if constexpr (std::is_same<T, MODE>::value){
                field_hw = &port_hw->MODER;
            }
            else if constexpr (std::same_as<T, OTYPE>){
                field_hw = &port_hw->OTYPER;
                mask = 0x01;
                mask_bit_len = 1;
            }
            else if constexpr (std::same_as<T, OSPEED>){
                field_hw = &port_hw->OSPEEDR;
            }
            else if constexpr (std::same_as<T, PUPD>){
                field_hw = &port_hw->PUPDR;
            }

            uint32_t tmp = *field_hw;
            for(int i = 0; i < sizeof(pins) * 8; ++i){
                if(pins & (1UL << i)){
                    tmp &= ~(mask << (i * mask_bit_len));
                    tmp |= std::to_underlying(prop) << (i * mask_bit_len);
                }
            }
            *field_hw = tmp;

            return OK;
        }

        static void set_level(PORT, pin_bitmask_t, bool);
        static void toggle_level(PORT, pin_bitmask_t);

    private:
        static constexpr bool IS_PIN(unsigned);
        static GPIO_TypeDef* get_port_ptr(PORT);
    };
}

#endif