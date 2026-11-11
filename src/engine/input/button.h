#pragma once

#include "hardware/gpio.h"
#include "pico/stdlib.h"

constexpr uint32_t RED_BTN_GPIO = 2;
constexpr uint32_t GREEN_BTN_GPIO = 3;
constexpr uint32_t BLUE_BTN_GPIO = 4;
constexpr uint32_t YELLOW_BTN_GPIO = 5;
constexpr uint32_t WHITE_BTN_GPIO = 6;

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
    bool current = false;
    bool previous = false;
};