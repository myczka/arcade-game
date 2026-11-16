#pragma once

#include <cstdint>


// Game-facing drawing interface for the buffered 128x160 ST7735 display.
class DisplayManager {
public:
    static constexpr std::uint8_t Width = 128;
    static constexpr std::uint8_t Height = 160;
    static constexpr std::uint16_t Black = 0x0000;
    static constexpr std::uint16_t White = 0xffff;
    static constexpr std::uint16_t Transparent = 0xF81F;

    void init();
    void clear(std::uint16_t color = Black);
    // Primitive drawing changes RAM only. Present after composing the frame.
    void present(std::int16_t x = 0, std::int16_t y = 0,
                 std::uint8_t width = Width,
                 std::uint8_t height = Height);
    void drawRectangle(std::int16_t x, std::int16_t y, std::uint8_t width,
                       std::uint8_t height, std::uint16_t color);
    void drawSprite(std::int16_t x, std::int16_t y, const std::uint16_t* pixels,
                    std::uint8_t width, std::uint8_t height);
    void drawText(std::uint8_t x, std::uint8_t y, const char* text,
                  std::uint16_t color = White, std::uint8_t scale = 1,
                  std::uint16_t background = Black);

    // Display the welcome screen and game title
    void showTitle();

    // Display the menu and highlight the selected game
    // 0 = Practice, 1 = Swimmy Fish.
    void showMenu(int selected);

    // Display instructions from "int selected" and "Press ENTER to start"
    // mode uses the same numbering as showMenu().
    void showInstructions(int mode);

    // Add Play again / Exit options to any game's results screen.
    // 0 = Play again selected, 1 = Exit selected.
    void showResultOptions(int selected);

};
