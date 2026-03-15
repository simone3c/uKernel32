#include <utility>

#include "uHAL/gpio.hpp"

using uHAL::GPIO;
using uHAL::LL_register;

// ---- auxiliary functions ----

template<typename T>
concept gpio_property = 
    std::same_as<T, GPIO::MODE> ||
    std::same_as<T, GPIO::OTYPE> ||
    std::same_as<T, GPIO::OSPEED> ||
    std::same_as<T, GPIO::PUPD>;

static GPIO_TypeDef* get_port_ptr(GPIO::PORT p) {
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

template<gpio_property T>
static uint32_t calculate_property(GPIO::pin_bitmask_t pins, T prop) {
    uint8_t mask_len = 2;

    if constexpr (std::same_as<T, GPIO::OTYPE>) {
        mask_len = 1;
    }

    uint32_t tmp = 0;
    for (unsigned i = 0; i < sizeof(pins) * 8; ++i) {
        if (pins & uHAL::BIT(i)) {
            const uint32_t mask = (1 << mask_len) - 1;
            tmp &= ~(mask << (i * mask_len));
            tmp |= std::to_underlying(prop) << (i * mask_len);
        }
    }
    return tmp;
};

// ---- specific registers abstraction ----

template<uint32_t OFFSET>
struct property_reg{
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};

    static void set(GPIO::PORT port, GPIO::pin_bitmask_t bitmask, GPIO::MODE m) {

        auto write = [](GPIO::PORT p, GPIO::pin_bitmask_t bm, uint32_t data){
            using enum GPIO::PORT;
            switch (p)
            {
            case A:
                regA.set(data, bm);
                break;
            case B:
                regB.set(data, bm);
                break;
            case C:
                regC.set(data, bm);
                break;
            case D:
                regD.set(data, bm);
                break;
            case E:
                regE.set(data, bm);
                break;
            case H:
                regH.set(data, bm);
                break;
            default:
                std::unreachable();
            }
        };
    
        const uint32_t data = calculate_property(bitmask, m);
        write(port, bitmask, data);
    }
};

using mode_reg = property_reg<0x0>;
using otype_reg = property_reg<0x4>;
using ospeed_reg = property_reg<0x8>;
using pupd_reg = property_reg<0xC>;


// ---- methods implementation ----

void GPIO::init() const {

    RCC->AHB1ENR |= 1;     // Enable GPIO clock for LED

    mode_reg::set(_port, _pins, _mode);
    otype_reg::set(_port, _pins, _mode);
    ospeed_reg::set(_port, _pins, _mode);
    pupd_reg::set(_port, _pins, _mode);
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
