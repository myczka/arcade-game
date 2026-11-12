/* Impliments the buzzer.h header file */

#include "buzzer.h"
#include "pico/hardware/pwm.h"

// create a new buzzer at a particular pin number
Buzzer::Buzzer(unsigned int pin) : pin(pin) {
    // figure out the slice and channel for the given pin
    this->slice = pwm_gpio_to_slice_num(pin);
    this->channel = pwm_gpio_to_channel(pin);
}

// Initialize the buzzer
void Buzzer::init() {
    gpio_set_function(pin, GPIO_FUNC_PWM);
}

// Play a tone at the specified frequency for the specified duration
void Buzzer::play(uint32_t frequency, uint32_t duration) {
    this->is_playing = true;
    this->stop_time = duration;
}

// Stop the buzzer
void Buzzer::stop() {
    this->is_playing = false;
    this->stop_time = 0;
}

// Update the buzzer state, stopping it if the duration has elapsed
void Buzzer::update() {

}