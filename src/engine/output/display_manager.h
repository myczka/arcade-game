#pragma once

#include "screen.h"

// Device-facing drawing only; game views choose text, layout, and colors.
class DisplayManager {
public:
    void init();
    void clear(std::uint16_t color = Screen::Black);
    void drawRectangle(std::uint8_t x, std::uint8_t y, std::uint8_t width,
                       std::uint8_t height, std::uint16_t color);
    void drawText(std::uint8_t x, std::uint8_t y, const char* text,
                  std::uint16_t color = Screen::White, std::uint8_t scale = 1,
                  std::uint16_t background = Screen::Black);

private:
    Screen screen_;
};
