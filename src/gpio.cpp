#include <utility>

#include "uHAL/gpio.hpp"

using uHAL::GPIO;

GPIO::GPIO(pin_t pinno, PORT port, MODE mode, OTYPE optye, OSPEED ospeed, PUPD pupd):
    _pinno(pinno), _port(port), _mode(mode), _otype(optye), _ospeed(ospeed), _pupd(pupd)
{}

uHAL::err_t GPIO::set_gpio_property(auto prop){
    return set_gpio_property(_port, _pinno, prop);
}

void GPIO::configure(){

    RCC->AHB1ENR |= 1;     // Enable GPIO clock for LED

    set_gpio_property(_port, _pinno, _mode);
    set_gpio_property(_port, _pinno, _otype);
    set_gpio_property(_port, _pinno, _ospeed);
    set_gpio_property(_port, _pinno, _pupd);
}

// --- static methods ---

GPIO GPIO::output_pin(
    pin_t pinno, 
    PORT port, 
    OTYPE otype, 
    OSPEED ospeed
){
    return GPIO(pinno, port, MODE::OUTPUT, otype, ospeed, PUPD::PULL_UP);
}

void GPIO::set_level(PORT port, pin_t pinno, bool l){
    GPIO_TypeDef* port_hw = get_port_ptr(port);

    port_hw->BSRR = (1U << pinno) << (l ? 0 : 16);
}

constexpr bool GPIO::IS_PIN(pin_t pin){
    return (pin & 0xFF) < 16;
}

GPIO_TypeDef* GPIO::get_port_ptr(PORT p){
    using enum PORT;

    switch (p){
        case A: return (GPIO_TypeDef*)GPIOA_BASE;
        case B: return (GPIO_TypeDef*)GPIOB_BASE;
        case C: return (GPIO_TypeDef*)GPIOC_BASE;
        case D: return (GPIO_TypeDef*)GPIOD_BASE;
        case E: return (GPIO_TypeDef*)GPIOE_BASE;
        case H: return (GPIO_TypeDef*)GPIOH_BASE;
        default: std::unreachable();
    }
}