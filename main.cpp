#include "uHAL/gpio.hpp"
#include "uHAL/systick.hpp"

static inline void spin(uint32_t count) {
    while (count--) (void)0;
}

void uHAL::systick::callback(void){
    uHAL::GPIO::toggle_level(uHAL::GPIO::PORT::A, uHAL::GPIO::PIN<5>);
}

int main(void) {
    
    uHAL::GPIO led = uHAL::GPIO::create_output(
        uHAL::GPIO::PORT::A, 
        uHAL::GPIO::PIN<5>
    );
    led.configure();

    auto sys = uHAL::systick::period_ms(100);
    sys.configure();


    // while(4){
    //     uHAL::GPIO::toggle_level(uHAL::GPIO::PORT::A, uHAL::GPIO::PIN<5>);
    //     spin(699999);
    //     uHAL::GPIO::toggle_level(uHAL::GPIO::PORT::A, uHAL::GPIO::PIN<5>);
    //     spin(699999);
    // }
}

