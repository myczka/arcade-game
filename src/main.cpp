#include <cstdint>

#include "engine/input/input_manager.h"
#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"
#include "game/main_menu.h"
#include "game/practice.h"
#include "pico/stdlib.h"

constexpr unsigned int BuzzerPin = 8;
constexpr int RemoteUp = 0x18;
constexpr int RemoteDown = 0x52;
constexpr int RemoteEnter = 0x1c;

enum class AppState {
    Title,
    Menu,
    Instructions,
    Practice
};

int main() {
    stdio_init_all();

    InputManager input;
    DisplayManager display;
    Buzzer buzzer(BuzzerPin);
    Practice practice(input, display, buzzer);
    AppState state = AppState::Title;
    MenuOption selected = MenuOption::Accuracy;
    bool screenNeedsUpdate = true;

    input.init();
    buzzer.init();
    display.init();

    while (true) {
        static std::uint64_t previousTime = time_us_64();
        const std::uint64_t currentTime = time_us_64();
        const float deltaTime =
            static_cast<float>(currentTime - previousTime) / 1000000.0f;
        previousTime = currentTime;

        input.update();
        buzzer.update();

        const int command = input.getRemoteCommand();

        switch (state) {
            case AppState::Title:
                if (screenNeedsUpdate) {
                    display.showTitle();
                    screenNeedsUpdate = false;
                }
                if (command >= 0) {
                    state = AppState::Menu;
                    screenNeedsUpdate = true;
                }
                break;

            case AppState::Menu:
                if (command == RemoteUp) {
                    selected = static_cast<MenuOption>(
                        (static_cast<int>(selected) + 2) % 3);
                    screenNeedsUpdate = true;
                } else if (command == RemoteDown) {
                    selected = static_cast<MenuOption>(
                        (static_cast<int>(selected) + 1) % 3);
                    screenNeedsUpdate = true;
                } else if (command == RemoteEnter) {
                    state = AppState::Instructions;
                    screenNeedsUpdate = true;
                }

                if (screenNeedsUpdate) {
                    display.showMenu(static_cast<int>(selected));
                    screenNeedsUpdate = false;
                }
                break;

            case AppState::Instructions:
                if (screenNeedsUpdate) {
                    display.showInstructions(static_cast<int>(selected));
                    screenNeedsUpdate = false;
                }
                if (command == RemoteEnter) {
                    if (selected == MenuOption::Accuracy) {
                        practice.start();
                        state = AppState::Practice;
                    } else {
                        state = AppState::Menu;
                    }
                    screenNeedsUpdate = true;
                }
                break;

            case AppState::Practice:
                practice.update(deltaTime);
                practice.render();
                if (practice.isFinished()) {
                    practice.start();
                }
                break;
        }

        sleep_ms(16);
    }
}
