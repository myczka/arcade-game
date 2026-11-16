#pragma once

#include "engine/output/display_manager.h"
#include <cstdio>
#include <cstdint>

// Shared game presentation helpers. These do not belong to the hardware layer.
// Compose in RAM, then present once when a complete view is ready.
namespace GameDisplay {
inline void heading(DisplayManager& display, const char* title) {
    display.clear();
    display.drawText(4, 8, title);
    display.drawRectangle(4, 24, 120, 1, 0x07ff);
}

inline void number(DisplayManager& display, std::uint8_t y,
                   const char* label, unsigned value) {
    char buffer[22];
    std::snprintf(buffer, sizeof(buffer), "%s%u", label, value);
    display.drawText(4, y, buffer);
}

inline void message(DisplayManager& display, const char* title, const char* text) {
    heading(display, title);
    display.drawText(4, 48, text);
    display.present();
}

inline void result(DisplayManager& display, const char* title,
                   unsigned value, const char* unit) {
    heading(display, title);
    char buffer[11];
    std::snprintf(buffer, sizeof(buffer), "%u", value);
    display.drawText(4, 48, buffer, DisplayManager::White, 2);
    display.drawText(4, 72, unit);
    display.drawText(4, 120, "IR: replay / menu");
    display.present();
}
}
