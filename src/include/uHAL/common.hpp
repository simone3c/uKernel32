#ifndef _uHAL_utils_
#define _uHAL_utils_

#include <stdint.h>

namespace uHAL{

    constexpr uint32_t BIT(unsigned b){
        return static_cast<uint32_t>(1) << b;
    }

    template<uint32_t addr>
    struct LL_register{
        void set(uint32_t val) const {
            *reinterpret_cast<volatile uint32_t*>(addr) = val;
        }

        void set(uint32_t val, uint32_t bitmask) const {
            
            *reinterpret_cast<volatile uint32_t*>(addr) &= ~bitmask;
            *reinterpret_cast<volatile uint32_t*>(addr) |= val;
        }

        void clear() const {
            set(0);
        }

        void clear(uint32_t bitmask) const {
            *reinterpret_cast<volatile uint32_t*>(addr) &= ~bitmask;
        }
    };

}

#endif