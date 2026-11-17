#pragma once

#include <stdint.h>


// Game-facing drawing interface for the buffered 128x160 ST7735 display.
class DisplayManager {
public:
    static constexpr uint8_t Width = 128;
    static constexpr uint8_t Height = 160;
    static constexpr uint16_t Black = 0x0000;
    static constexpr uint16_t White = 0xffff;
    static constexpr uint16_t Transparent = 0xF81F;

    void init();
    void clear(uint16_t color = Black);
    // Primitive drawing changes RAM only. Present after composing the frame.
    void present(int16_t x = 0, int16_t y = 0, uint8_t width = Width, uint8_t height = Height);
    void drawRectangle(int16_t x, int16_t y, uint8_t width, uint8_t height, uint16_t color);
    void drawSprite(int16_t x, int16_t y, const uint16_t* pixels, uint8_t width, uint8_t height);
    void drawText(uint8_t x, uint8_t y, const char* text, uint16_t color = White, uint8_t scale = 1, uint16_t background = Black);

};
