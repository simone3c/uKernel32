#ifndef _uHAL_utils_
#define _uHAL_utils_

#include <stdint.h>

namespace uHAL{

    constexpr uint32_t BIT(unsigned b){
        return static_cast<uint32_t>(1) << b;
    }

}

#endif