#ifndef _CORTEX_M4_
#define _CORTEX_M4_


#include <cstdint>

// --- MEMORY MAPPED PERIPHERALS STRUCT ----

typedef struct {

    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;

} systick_typedef;

// -----------------------------------------
// ---- MEMORY MAPPED PERIPHERALS ADDRESS ----

#define SYSTICK (0xE000E010)

#endif