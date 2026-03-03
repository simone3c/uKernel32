#include <stdint.h>

#include "stm32f401xe.h"

#define RETURN_IF_ERROR(left, ok_value) if(ret = (left); ret != (ok_value)) { return ret }

namespace uHAL{

    struct GPIO{
        uint8_t _pinno;
        enum PORT{A, B, C, D, E, H} _port;
        enum MODE{INPUT, OUTPUT, ALTERNATE, ANALOG} _mode;
        enum OTYPE{PUSH_PULL, OPEN_DRAIN} _otype;
        enum OSPEED{LOW, MEDIUM, HIGH, VERY_HIGH} _ospeed;
        enum PUPD{PULL_UP, PULL_DOWN} _pupd;
        
        // TODO alternate func e lock

        GPIO(uint8_t pinno, PORT port, MODE mode, OTYPE optye, OSPEED ospeed, PUPD pupd):
            _pinno(pinno), _port(port), _mode(mode), _otype(optye), _ospeed(ospeed), _pupd(pupd)
        {}

        static GPIO output_pin(uint8_t pinno, PORT port, OTYPE otype = PUSH_PULL, OSPEED ospeed = LOW){
            return GPIO(pinno, port, OUTPUT, otype, ospeed, PULL_UP);
        }

        void configure(){

            RCC->AHB1ENR |= 1;     // Enable GPIO clock for LED

            // todo get_port
            GPIO_TypeDef* port_hw = (GPIO_TypeDef*)GPIOA_BASE;

            port_hw->MODER &= ~(0x3 << _pinno * 2); 
            port_hw->MODER |= (_mode << _pinno * 2); 

            port_hw->OTYPER &= ~(1 << _pinno);
            port_hw->OTYPER |= (_otype << _pinno);

            port_hw->OSPEEDR &= ~(3 << _pinno * 2);
            port_hw->OSPEEDR |= (_ospeed << _pinno * 2);

            port_hw->PUPDR &= ~(3 << _pinno * 2);
            port_hw->PUPDR |= (_pupd << _pinno * 2);
        }

        static void set_level(PORT port, uint8_t pinno, bool l){
            // todo get_port
            GPIO_TypeDef* port_hw = (GPIO_TypeDef*)GPIOA_BASE;

            port_hw->BSRR = (1U << pinno) << (l ? 0 : 16);
        }
    };

}