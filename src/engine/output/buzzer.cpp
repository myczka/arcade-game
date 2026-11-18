#include "buzzer.h"

#include <algorithm>

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/time.h"

Buzzer::Buzzer(unsigned int pin) : pin_(pin) {
    slice_ = pwm_gpio_to_slice_num(pin_);
    channel_ = pwm_gpio_to_channel(pin_);
    is_playing_ = false;
    stop_time_ = 0;
}

void Buzzer::init() {
    gpio_set_function(pin_, GPIO_FUNC_PWM);
    pwm_set_chan_level(slice_, channel_, 0);
    pwm_set_enabled(slice_, false);
}

void Buzzer::play(uint32_t frequency, uint32_t duration, uint8_t volume, uint8_t release_volume) {
    if (frequency == 0 || duration == 0 || volume == 0) {
        stop();
        return;
    }

    sweeping_ = false;
    sweep_start_frequency_ = std::clamp<uint32_t>(frequency, 20, 20000);
    start_volume_ = volume;
    release_volume_ = release_volume;
    setTone(frequency, volume, true);
    pwm_set_enabled(slice_, true);

    is_playing_ = true;
    start_time_ = time_us_64();
    stop_time_ = start_time_ + static_cast<uint64_t>(duration) * 1000u;
}

void Buzzer::setTone(uint32_t frequency, uint8_t volume, bool restart) {
    // Keep divider and wrap calculations within the PWM's supported range.
    frequency = std::clamp<uint32_t>(frequency, 20, 20000);
    const uint32_t clock_hz = clock_get_hz(clk_sys);
    const uint64_t denominator = static_cast<uint64_t>(frequency) * 65536u;
    const float divider = std::clamp(static_cast<float>((clock_hz + denominator - 1) / denominator), 1.0f, 255.9375f);
    const uint32_t top = std::min<uint32_t>(static_cast<uint32_t>(static_cast<double>(clock_hz) / (static_cast<double>(divider) * frequency) - 1.0), 65535u);

    if (restart) {
        // Start each new sound using the original, working PWM setup.
        pwm_config config = pwm_get_default_config();
        pwm_config_set_clkdiv(&config, divider);
        pwm_config_set_wrap(&config, top);
        pwm_init(slice_, &config, false);
    } else {
        // Change a running sweep without resetting its counter.
        pwm_set_clkdiv(slice_, divider);
        pwm_set_wrap(slice_, top);
    }
    // Lower duty reduces the tone's fundamental; GPIO voltage stays fixed.
    pwm_set_chan_level(slice_, channel_, (top / 2) * volume / 255u);
}

void Buzzer::sweep(uint32_t start_frequency, uint32_t end_frequency, uint32_t duration) {
    if (start_frequency == 0 || end_frequency == 0 || duration == 0) {
        stop();
        return;
    }
    sweep_start_frequency_ = std::clamp<uint32_t>(start_frequency, 20, 20000);
    sweep_end_frequency_ = std::clamp<uint32_t>(end_frequency, 20, 20000);
    play(sweep_start_frequency_, duration);
    sweeping_ = true;
}

void Buzzer::stop() {
    pwm_set_chan_level(slice_, channel_, 0);
    pwm_set_enabled(slice_, false);
    is_playing_ = false;
    sweeping_ = false;
    stop_time_ = 0;
}

void Buzzer::update() {
    if (!is_playing_) {
        return;
    }

    const uint64_t now = time_us_64();
    if (now >= stop_time_) {
        stop();
        return;
    }
    
    if (sweeping_ || release_volume_ < 255) {
        const uint64_t duration = stop_time_ - start_time_;
        const uint64_t elapsed = now - start_time_;
        uint32_t frequency = sweep_start_frequency_;

        if (sweeping_) {
            const float progress = static_cast<float>(elapsed) / duration;
            frequency = static_cast<uint32_t>(sweep_start_frequency_ + (static_cast<float>(sweep_end_frequency_) - sweep_start_frequency_) * progress);
        }

        uint8_t volume = start_volume_;
        // Use a 20 ms ping (shorter for very short sounds), then snap down.
        const auto ping_duration = std::min<uint64_t>(20000, duration / 4);
        if (release_volume_ < 255 && elapsed >= ping_duration) {
            const auto release = std::min(release_volume_, start_volume_);
            volume = static_cast<uint8_t>(release * (stop_time_ - now) / (duration - ping_duration));
        }
        setTone(frequency, volume);
    }
}
