#include "tutorial.h"

Tutorial::Tutorial(DisplayManager& display) : display_(display) {}

void Tutorial::render(MenuOption game) {
    constexpr uint16_t Cyan = 0x07FF;
    constexpr uint16_t Yellow = 0xFFE0;

    display_.clear();
    display_.drawRectangle(2, 2, 124, 156, Cyan);
    display_.drawRectangle(3, 3, 122, 154, DisplayManager::Black);
    display_.drawRectangle(12, 32, 104, 1, Cyan);

    if (game == MenuOption::Practice) {
        display_.drawText(40, 15, "PRACTICE", Cyan);
        display_.drawText(10, 50, "Press the matching");
        display_.drawText(19, 65, "colored button");
        display_.drawText(16, 90, "when notes reach");
        display_.drawText(19, 105, "the white zone.");
    } else if (game == MenuOption::SwimmyFish) {
        display_.drawText(31, 15, "SWIMMY FISH", Cyan);
        display_.drawText(10, 55, "Swim through gaps.");
        display_.drawText(16, 80, "Avoid the pipes.");
        display_.drawText(7, 105, "Pass pipes to score");
    } else {
        return;
    }

    display_.drawRectangle(12, 130, 104, 22, Yellow);
    display_.drawRectangle(13, 131, 102, 20, DisplayManager::Black);
    display_.drawText(22, 137, "ENTER to start", Yellow);
    display_.present();
}
