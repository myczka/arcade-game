#pragma once

#include "hardware/gpio.h"
#include "pico/stdlib.h"

enum class Color {
    Red,
    Green,
    Blue,
    Yellow,
    White
};

struct Button {
    Color color;
    uint32_t pin;
    bool current;
    bool previous;
};