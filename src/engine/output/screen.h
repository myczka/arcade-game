#pragma once

#include <cstdint>

// Drawing primitives for the 128x160 ST7735 display. Coordinates are pixels.
class Screen {
public:
    static constexpr std::uint8_t Width = 128;
    static constexpr std::uint8_t Height = 160;
    static constexpr std::uint16_t Black = 0x0000;
    static constexpr std::uint16_t White = 0xffff;

    void init();
    void clear(std::uint16_t color = Black);
    void rectangle(std::uint8_t x, std::uint8_t y,
                   std::uint8_t width, std::uint8_t height, std::uint16_t color);
    // Single-line printable ASCII; text is clipped to the display.
    void text(std::uint8_t x, std::uint8_t y, const char* value,
              std::uint16_t color = White, std::uint8_t scale = 1,
              std::uint16_t background = Black);
};
