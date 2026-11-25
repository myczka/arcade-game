#pragma once

#include "hardware/gpio.h"
#include "pico/stdlib.h"

constexpr uint32_t RED_BTN_GPIO = 2;
constexpr uint32_t GREEN_BTN_GPIO = 3;
constexpr uint32_t BLUE_BTN_GPIO = 4;
constexpr uint32_t YELLOW_BTN_GPIO = 5;

enum class Color {
    Red,
    Green,
    Blue,
    Yellow,
    Count
};

struct Button {
    uint32_t pin;
    bool current = false;
    bool previous = false;
    // Only the timer sampler changes these debounce fields after init().
    bool candidate = false;
    uint32_t candidateSinceUs = 0;
};
