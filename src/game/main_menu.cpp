#include "main_menu.h"
#include "display_helpers.h"

void MainMenu::render(DisplayManager& display, MenuOption selected) {
    GameDisplay::heading(display, "SHOW ME YOUR SKILLS");
    const char* labels[] = {"Accuracy", "Reaction", "Memory"};
    for (unsigned i = 0; i < 3; ++i) {
        const auto y = static_cast<std::uint8_t>(40 + i * 28);
        const bool active = i == static_cast<unsigned>(selected);
        const auto background = active ? Screen::White : Screen::Black;
        const auto foreground = active ? Screen::Black : Screen::White;
        display.drawRectangle(4, y - 4, 120, 20, background);
        display.drawText(10, y, labels[i], foreground, 1, background);
    }
    display.drawText(4, 136, "IR: choose / confirm");
}
