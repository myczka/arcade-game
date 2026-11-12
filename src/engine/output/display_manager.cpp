#include "display_manager.h"

void DisplayManager::init() {
    screen_.init();
}

void DisplayManager::clear(std::uint16_t color) {
    screen_.clear(color);
}

void DisplayManager::drawRectangle(std::uint8_t x, std::uint8_t y,
                                    std::uint8_t width, std::uint8_t height,
                                    std::uint16_t color) {
    screen_.rectangle(x, y, width, height, color);
}

void DisplayManager::drawText(std::uint8_t x, std::uint8_t y, const char* text,
                              std::uint16_t color, std::uint8_t scale,
                              std::uint16_t background) {
    screen_.text(x, y, text, color, scale, background);
}
