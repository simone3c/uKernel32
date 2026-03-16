#include <utility>

#include "uHAL/gpio.hpp"

using uHAL::gpio;
using uHAL::LL_register;

// ---- auxiliary functions ----

template<typename T>
concept gpio_property = 
    std::same_as<T, gpio::mode_t> ||
    std::same_as<T, gpio::otype_t> ||
    std::same_as<T, gpio::osped_t> ||
    std::same_as<T, gpio::pupd_t>;

// Auxiliry function that calculates the correct bitmask to set a "gpio_property"
// based on T and on the interested pins
template<gpio_property T>
static uint32_t calc_gpio_property(gpio::pin_bitmask_t pins, T prop) {
    uint8_t mask_len = 2;

    if constexpr (std::same_as<T, gpio::otype_t>) {
        mask_len = 1;
    }

    uint32_t tmp = 0;
    for (unsigned i = 0; i < sizeof(pins) * 8; ++i) {
        if (pins & uHAL::BIT(i)) {
            tmp |= std::to_underlying(prop) << (i * mask_len);
        }
    }
    return tmp;
};

// ---- specific registers abstraction ----

// Models 4 different registers within a port (MODER, OTYPER, OSPEEDR, PUPDR)
// which are accessed in the same way.
// 7 different physical registers are available, depending on the port
template<uint32_t OFFSET>
struct property_reg{
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};

    static void set(gpio::port_t port, gpio::pin_bitmask_t bitmask, gpio::mode_t m) {

        auto write = [](gpio::port_t p, gpio::pin_bitmask_t bm, uint32_t data){
            using enum gpio::port_t;
            switch (p)
            {
            case A:
                regA.set(bm, data);
                break;
            case B:
                regB.set(bm, data);
                break;
            case C:
                regC.set(bm, data);
                break;
            case D:
                regD.set(bm, data);
                break;
            case E:
                regE.set(bm, data);
                break;
            case H:
                regH.set(bm, data);
                break;
            default:
                std::unreachable();
            }
        };
    
        const uint32_t data = calc_gpio_property(bitmask, m);
        write(port, bitmask, data);
    }
};

using mode_reg = property_reg<0x0>;
using otype_reg = property_reg<0x4>;
using ospeed_reg = property_reg<0x8>;
using pupd_reg = property_reg<0xC>;

// Models the ODR registers
// 7 different physical registers are available, depending on the port
struct odr_reg{
    static constexpr uint32_t OFFSET = 0x14;
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};

    static void write(gpio::port_t port, gpio::pin_bitmask_t bm, uint32_t data){
        using enum gpio::port_t;
        switch (port)
        {
        case A:
            regA.set(bm, data);
            break;
        case B:
            regB.set(bm, data);
            break;
        case C:
            regC.set(bm, data);
            break;
        case D:
            regD.set(bm, data);
            break;
        case E:
            regE.set(bm, data);
            break;
        case H:
            regH.set(bm, data);
            break;
        default:
            std::unreachable();
        }
    };

    static uint32_t read(gpio::port_t port, gpio::pin_bitmask_t bm){
        using enum gpio::port_t;
        switch (port)
        {
        case A: return regA.read(bm);
        case B: return regB.read(bm);
        case C: return regC.read(bm);
        case D: return regD.read(bm);
        case E: return regE.read(bm);
        case H: return regH.read(bm);
        default: std::unreachable();
        }
    };
};

// models the BSRR registers
// 7 different physical registers are available, depending on the port
struct bsrr_reg{
    static constexpr uint32_t OFFSET = 0x18;
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};
    
    static void set(gpio::port_t p, gpio::pin_bitmask_t pins, gpio::pin_level_t lv){
        const uint32_t data = 
            static_cast<uint32_t>(pins) << (lv == gpio::pin_level_t::HIGH ? 0 : 16);
        write(p, pins, data);
    }

    static void toggle(gpio::port_t port, gpio::pin_bitmask_t pins){
        const uint32_t odr = odr_reg::read(port, pins);
        const uint32_t data = ((odr & pins) << 16) | (~odr & pins);
        write(port, pins, data);
    }

    private:
    static constexpr void write(gpio::port_t port, gpio::pin_bitmask_t bm, uint32_t data){
        using enum gpio::port_t;
        switch (port)
        {
        case A:
            regA.set(bm, data);
            break;
        case B:
            regB.set(bm, data);
            break;
        case C:
            regC.set(bm, data);
            break;
        case D:
            regD.set(bm, data);
            break;
        case E:
            regE.set(bm, data);
            break;
        case H:
            regH.set(bm, data);
            break;
        default:
            std::unreachable();
        }
    };
};

// ---- methods implementation ----

void gpio::apply() const {

    RCC->AHB1ENR |= 1;     // Enable GPIO clock for LED

    mode_reg::set(port, pins, mode);
    otype_reg::set(port, pins, mode);
    ospeed_reg::set(port, pins, mode);
    pupd_reg::set(port, pins, mode);
    set_level(port, pins, pin_level_t::LOW);
}

void gpio::set_level(pin_level_t lv) const {
    set_level(port, pins, lv);
}

void gpio::toggle_level() const {
    toggle_level(port, pins);
}

// --- static methods ---

gpio gpio::create_output(
    port_t port, 
    pin_bitmask_t bitmask,
    otype_t otype, 
    osped_t ospeed
){
    return gpio{
        .pins = bitmask, 
        .port = port, 
        .mode = mode_t::output, 
        .otype = otype, 
        .ospeed = ospeed, 
        .pupd = pupd_t::none
    };
}

void gpio::set_level(port_t port, pin_bitmask_t bitmask, pin_level_t l){
    bsrr_reg::set(port, bitmask, l);
}

void gpio::toggle_level(port_t port, pin_bitmask_t bitmask){
    bsrr_reg::toggle(port, bitmask);
}
