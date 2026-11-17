#include "main_menu.h"

void MainMenu::renderTitle(DisplayManager& display) {
    constexpr uint16_t Cyan = 0x07FF;

    display.clear();
    display.drawRectangle(2, 2, 124, 156, Cyan);
    display.drawRectangle(3, 3, 122, 154, DisplayManager::Black);
    display.drawRectangle(12, 48, 104, 32, Cyan);
    display.drawRectangle(13, 49, 102, 30, DisplayManager::Black);
    display.drawText(37, 60, "SWIMMY FISH", Cyan);
    display.drawText(31, 100, "Press ENTER");
    display.drawText(31, 115, "to continue");
    display.present();
}

void MainMenu::render(DisplayManager& display, MenuOption selected) {
    constexpr uint16_t Cyan = 0x07FF;
    constexpr uint16_t Yellow = 0xFFE0;

    display.clear();
    display.drawRectangle(2, 2, 124, 156, Cyan);
    display.drawRectangle(3, 3, 122, 154, DisplayManager::Black);
    display.drawText(34, 15, "CHALLENGES", Cyan);
    display.drawRectangle(12, 32, 104, 1, Cyan);

    uint16_t practice_color = DisplayManager::White;
    uint16_t swimmy_color = DisplayManager::White;
    if (selected == MenuOption::Practice) {
        practice_color = Yellow;
        display.drawRectangle(12, 45, 104, 24, Yellow);
        display.drawRectangle(13, 46, 102, 22, DisplayManager::Black);
    } else if (selected == MenuOption::SwimmyFish) {
        swimmy_color = Yellow;
        display.drawRectangle(12, 75, 104, 24, Yellow);
        display.drawRectangle(13, 76, 102, 22, DisplayManager::Black);
    }

    display.drawText(40, 53, "PRACTICE", practice_color);
    display.drawText(40, 83, "SWIMMY FISH", swimmy_color);
    display.drawText(19, 140, "ENTER to select");
    display.present();
}
