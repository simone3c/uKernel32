#ifndef _uHAL_utils_
#define _uHAL_utils_

#include <stdint.h>

namespace uHAL{

    constexpr uint32_t BIT(unsigned b){
        return static_cast<uint32_t>(1) << b;
    }

    // performs:
    // 1. reg &= ~mask;
    // 2. reg |= val & mask;
    // Note that the value is automatically masked with the mask value itself 
    // to write only in the cleared bits
    inline void LL_write_reg(volatile uint32_t& reg, uint32_t val, uint32_t mask) {
        reg &= ~mask;
        reg |= val & mask;
    }

    // performs:
    // 1. reg = val;
    // Note that this overload overwrites the whole register
    inline void LL_write_reg(volatile uint32_t& reg, uint32_t val) {
        reg = val;
    }

}

#endif