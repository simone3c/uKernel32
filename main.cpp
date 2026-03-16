#include "uHAL/gpio.hpp"
#include "uHAL/systick.hpp"

static inline void spin(uint32_t count) {
    while (count--) (void)0;
}

void systick_cb(void){
    uHAL::gpio::toggle_level(uHAL::gpio::port_t::A, uHAL::gpio::PIN<5>);
}

int main(void) {
    
    uHAL::gpio led = uHAL::gpio::create_output(
        uHAL::gpio::port_t::A, 
        uHAL::gpio::PIN<5> // todo test | PIN<6>
    );
    led.apply();

    auto sys = uHAL::systick::period_ms(2000, systick_cb);
    sys.apply();

    while(4){
        // uHAL::gpio::toggle_level(uHAL::gpio::port_t::A, uHAL::gpio::PIN<5>);
        // spin(99999);
        // uHAL::gpio::toggle_level(uHAL::gpio::port_t::A, uHAL::gpio::PIN<5>);
        // spin(99999);
    }
}

