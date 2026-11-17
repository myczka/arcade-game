#pragma once

#include "engine/output/display_manager.h"

// Menu selection is game state, independent of the display hardware.
enum class MenuOption { SwimmyFish, Practice, Results, Count };

class MainMenu {
public:
    void renderTitle(DisplayManager& display);
    void render(DisplayManager& display, MenuOption selected);
private:
    static constexpr uint16_t Background = 0x5B7C;
};
