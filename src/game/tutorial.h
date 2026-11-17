#pragma once

#include "engine/output/display_manager.h"
#include "main_menu.h"

class Tutorial {
public:
    explicit Tutorial(DisplayManager& display);
    void render(MenuOption game);

private:
    DisplayManager& display_;
};
