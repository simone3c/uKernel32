#include <utility>

#include "uHAL/gpio.hpp"

using uHAL::GPIO;

GPIO_TypeDef* get_port_ptr(GPIO::PORT p) {
    using enum GPIO::PORT;

    switch (p) {
        case A: return (GPIO_TypeDef*)GPIOA_BASE;
        case B: return (GPIO_TypeDef*)GPIOB_BASE;
        case C: return (GPIO_TypeDef*)GPIOC_BASE;
        case D: return (GPIO_TypeDef*)GPIOD_BASE;
        case E: return (GPIO_TypeDef*)GPIOE_BASE;
        case H: return (GPIO_TypeDef*)GPIOH_BASE;
        default: std::unreachable();
    }
}

template<typename T>
    requires
    std::same_as<T, GPIO::MODE> ||
    std::same_as<T, GPIO::OTYPE> ||
    std::same_as<T, GPIO::OSPEED> ||
    std::same_as<T, GPIO::PUPD>
void set_property_enum(GPIO::PORT p, GPIO::pin_bitmask_t pins, T prop) {
    GPIO_TypeDef* port_hw = get_port_ptr(p);
    volatile uint32_t* field_hw = nullptr;

    uint8_t mask_len = 2;

    if constexpr (std::same_as<T, GPIO::MODE>) {
        field_hw = &port_hw->MODER;
    }
    else if constexpr (std::same_as<T, GPIO::OTYPE>) {
        field_hw = &port_hw->OTYPER;
        mask_len = 1;
    }
    else if constexpr (std::same_as<T, GPIO::OSPEED>) {
        field_hw = &port_hw->OSPEEDR;
    }
    else if constexpr (std::same_as<T, GPIO::PUPD>) {
        field_hw = &port_hw->PUPDR;
    }

    uint32_t tmp = *field_hw;
    for (unsigned i = 0; i < sizeof(pins) * 8; ++i) {
        if (pins & uHAL::BIT(i)) {
            const uint32_t mask = (1 << mask_len) - 1;
            tmp &= ~(mask << (i * mask_len));
            tmp |= std::to_underlying(prop) << (i * mask_len);
        }
    }
    *field_hw = tmp;
};

void GPIO::init() const {

    RCC->AHB1ENR |= 1;     // Enable GPIO clock for LED

    set_property_enum(_port, _pins, _mode);
    set_property_enum(_port, _pins, _otype);
    set_property_enum(_port, _pins, _ospeed);
    set_property_enum(_port, _pins, _pupd);
}

void GPIO::set_level(bool l) const {
    set_level(_port, _pins, l);
}

void GPIO::toggle_level() const {
    toggle_level(_port, _pins);
}

// --- static methods ---

GPIO GPIO::create_output(
    PORT port, 
    pin_bitmask_t pinno,
    OTYPE otype, 
    OSPEED ospeed
){
    return GPIO{
        ._pins = pinno, 
        ._port = port, 
        ._mode = MODE::OUTPUT, 
        ._otype = otype, 
        ._ospeed = ospeed, 
        ._pupd = PUPD::NONE
    };
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
