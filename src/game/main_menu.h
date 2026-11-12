#pragma once

#include "engine/output/display_manager.h"

// Menu selection is game state, independent of the display hardware.
enum class MenuOption { Accuracy, Reaction, Memory };

class MainMenu {
public:
    void render(DisplayManager& display, MenuOption selected);
};
