#include "main_menu.h"

void MainMenu::render(DisplayManager& display, MenuOption selected) {
    display.showMenu(static_cast<int>(selected));
}
