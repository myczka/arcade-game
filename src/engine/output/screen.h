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
    // Send a buffered region to the display; defaults to the whole screen.
    void present(std::uint8_t x = 0, std::uint8_t y = 0,
                 std::uint8_t width = Width, std::uint8_t height = Height);
    void rectangle(std::uint8_t x, std::uint8_t y,
                   std::uint8_t width, std::uint8_t height, std::uint16_t color);
    // Signed positions allow sprites to move partly outside the display.
    void sprite(std::int16_t x, std::int16_t y, const std::uint16_t* pixels,
                std::uint8_t width, std::uint8_t height,
                std::uint16_t transparent);
    // Single-line printable ASCII; text is clipped to the display.
    void text(std::uint8_t x, std::uint8_t y, const char* value,
              std::uint16_t color = White, std::uint8_t scale = 1,
              std::uint16_t background = Black);
};
