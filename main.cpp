#include "uHAL/gpio.hpp"
#include "uHAL/systick.hpp"

static inline void spin(uint32_t count) {
    while (count--) (void)0;
}

void systick_cb(void){
    uHAL::GPIO::toggle_level(uHAL::GPIO::PORT::A, uHAL::GPIO::PIN<5>);
}

int main(void) {
    
    uHAL::GPIO led = uHAL::GPIO::create_output(
        uHAL::GPIO::PORT::A, 
        uHAL::GPIO::PIN<5> // todo test | PIN<6>
    );
    led.init();

    auto sys = uHAL::systick::period_ms(500, systick_cb);
    sys.init();

    while(4){
        // uHAL::GPIO::toggle_level(uHAL::GPIO::PORT::A, uHAL::GPIO::PIN<5>);
        // spin(99999);
        // uHAL::GPIO::toggle_level(uHAL::GPIO::PORT::A, uHAL::GPIO::PIN<5>);
        // spin(99999);
    }
}

