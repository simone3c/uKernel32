#include <utility>

#include "uHAL/gpio.hpp"

using uHAL::GPIO;

GPIO::GPIO(PORT port, pin_bitmask_t pins, MODE mode, OTYPE optye, OSPEED ospeed, PUPD pupd):
    _pins(pins), _port(port), _mode(mode), _otype(optye), _ospeed(ospeed), _pupd(pupd)
{}

uHAL::err_t GPIO::set_gpio_property(auto prop){
    return set_property(_port, _pins, prop);
}

void GPIO::configure(){

    RCC->AHB1ENR |= 1;     // Enable GPIO clock for LED

    set_property(_port, _pins, _mode);
    set_property(_port, _pins, _otype);
    set_property(_port, _pins, _ospeed);
    set_property(_port, _pins, _pupd);
}

// --- static methods ---

GPIO GPIO::create_output(
    PORT port, 
    pin_bitmask_t pinno,
    OTYPE otype, 
    OSPEED ospeed
){
    return GPIO(port, pinno, MODE::OUTPUT, otype, ospeed, PUPD::NONE);
}

void GPIO::set_level(PORT port, pin_bitmask_t pins, bool l){
    GPIO_TypeDef* port_hw = get_port_ptr(port);

    port_hw->BSRR |= ((uint32_t)pins) << (l ? 0 : 16);
}

void GPIO::toggle_level(PORT port, pin_bitmask_t pins){
    GPIO_TypeDef* port_hw = get_port_ptr(port);

    uint32_t odr = port_hw->ODR;

    port_hw->BSRR = (odr & pins) << 16 | (~odr & pins);
}

constexpr bool GPIO::IS_PIN(unsigned pin){
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