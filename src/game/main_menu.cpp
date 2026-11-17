#include "main_menu.h"
#include "assets/SWIMMY_FISH.h"

void MainMenu::renderTitle(DisplayManager& display) {
    constexpr uint16_t Cyan = 0x07FF;

    display.clear(Background);
    display.drawRectangle(2, 2, 124, 156, Cyan);
    display.drawRectangle(3, 3, 122, 154, Background);
    display.drawSprite((DisplayManager::Width - SWIMMY_FISH_WIDTH) / 2, 44, SWIMMY_FISH, SWIMMY_FISH_WIDTH, SWIMMY_FISH_HEIGHT);
    display.drawText(31, 100, "Press ENTER", DisplayManager::White, 1, Background);
    display.drawText(31, 115, "to continue", DisplayManager::White, 1, Background);
    display.present();
}

void MainMenu::render(DisplayManager& display, MenuOption selected) {
    constexpr uint16_t Cyan = 0x07FF;
    constexpr uint16_t Orange = 0xFD20;

    display.clear(Background);
    display.drawRectangle(2, 2, 124, 156, Cyan);
    display.drawRectangle(3, 3, 122, 154, Background);
    display.drawText(34, 15, "CHALLENGES", Cyan, 1, Background);
    display.drawRectangle(12, 32, 104, 1, Cyan);

    uint16_t practice_color = Orange;
    uint16_t swimmy_color = Orange;
    uint16_t results_color = Orange;
    if (selected == MenuOption::SwimmyFish) {
        swimmy_color = DisplayManager::White;
        display.drawRectangle(12, 45, 104, 24, DisplayManager::White);
        display.drawRectangle(13, 46, 102, 22, Background);
    } else if (selected == MenuOption::Practice) {
        practice_color = DisplayManager::White;
        display.drawRectangle(12, 75, 104, 24, DisplayManager::White);
        display.drawRectangle(13, 76, 102, 22, Background);
    } else if (selected == MenuOption::Results) {
        results_color = DisplayManager::White;
        display.drawRectangle(12, 105, 104, 24, DisplayManager::White);
        display.drawRectangle(13, 106, 102, 22, Background);
    }

    display.drawText(40, 53, "SWIMMY FISH", swimmy_color, 1, Background);
    display.drawText(40, 83, "PRACTICE", practice_color, 1, Background);
    display.drawText(43, 113, "RESULTS", results_color, 1, Background);
    display.drawText(19, 140, "ENTER to select", DisplayManager::White, 1, Background);
    display.present();
}
