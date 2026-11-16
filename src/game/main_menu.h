#pragma once

#include "engine/output/display_manager.h"

// Menu selection is game state, independent of the display hardware.
enum class MenuOption { Practice, SwimmyFish, Count };

class MainMenu {
public:
    void render(DisplayManager& display, MenuOption selected);
};
