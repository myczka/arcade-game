#include "buzzer.h"

#include <algorithm>

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/time.h"

Buzzer::Buzzer(unsigned int pin) : pin(pin) {
    slice = pwm_gpio_to_slice_num(pin);
    channel = pwm_gpio_to_channel(pin);
    is_playing = false;
    stop_time = 0;
}

void Buzzer::init() {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    pwm_set_chan_level(slice, channel, 0);
    pwm_set_enabled(slice, false);
}

void Buzzer::play(std::uint32_t frequency, std::uint32_t duration) {
    if (frequency == 0 || duration == 0) {
        stop();
        return;
    }

    const std::uint32_t clock_hz = clock_get_hz(clk_sys);
    const std::uint64_t denominator =
        static_cast<std::uint64_t>(frequency) * 65536u;
    const float divider = std::clamp(
        static_cast<float>((clock_hz + denominator - 1) / denominator),
        1.0f,
        255.9375f);
    const std::uint32_t top = std::min<std::uint32_t>(
        static_cast<std::uint32_t>(
            static_cast<double>(clock_hz) /
                (static_cast<double>(divider) * frequency) -
            1.0),
        65535u);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, divider);
    pwm_config_set_wrap(&config, top);
    pwm_init(slice, &config, false);
    pwm_set_chan_level(slice, channel, top / 2);
    pwm_set_enabled(slice, true);

    is_playing = true;
    stop_time = time_us_64() +
                static_cast<std::uint64_t>(duration) * 1000u;
}

void Buzzer::stop() {
    pwm_set_chan_level(slice, channel, 0);
    pwm_set_enabled(slice, false);
    is_playing = false;
    stop_time = 0;
}

void Buzzer::update() {
    if (is_playing && time_us_64() >= stop_time) {
        stop();
    }
}