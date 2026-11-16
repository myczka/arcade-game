#include <cstdint>
#include <cstdio>

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
constexpr int RemoteLeft = 0x08;
constexpr int RemoteRight = 0x5a;
constexpr int RemoteStar = 0x16;
constexpr int RemoteHash = 0x42;

bool isRemoteUp(int command) {
    return command == 0x18 || command == 0x46;
}

bool isRemoteDown(int command) {
    return command == 0x52 || command == 0x15;
}

bool isRemoteEnter(int command) {
    return command == RemoteEnter || command == 0x1c;
}

bool isRemoteExit(int command) {
    return command == RemoteStar || command == RemoteHash;
}

enum class AppState {
    Title,
    Menu,
    Instructions,
    Practice
};
constexpr std::uint64_t FrameDurationUs = 1000000 / 30;
constexpr int MenuOptionCount = static_cast<int>(MenuOption::Count);

int main() {
    stdio_init_all();

    InputManager input;
    DisplayManager display;
    Buzzer buzzer(BuzzerPin);
    Practice practice(input, display, buzzer);
    AppState state = AppState::Title;
    MenuOption selected = MenuOption::Practice;
    bool screenNeedsUpdate = true;

    input.init();
    buzzer.init();
    display.init();

    std::uint64_t previousTime = time_us_64();
    while (true) {
        const std::uint64_t currentTime = time_us_64();
        const float deltaTime =
            static_cast<float>(currentTime - previousTime) / 1000000.0f;
        previousTime = currentTime;

        input.update();
        buzzer.update();

        const int command = input.getRemoteCommand();
        if (command >= 0) {
            std::printf("IR command: 0x%02X\n", command);
        }

        switch (state) {
            case AppState::Title:
                if (screenNeedsUpdate) {
                    display.showTitle();
                    screenNeedsUpdate = false;
                }
                if (isRemoteEnter(command)) {
                    state = AppState::Menu;
                    screenNeedsUpdate = true;
                }
                break;

            case AppState::Menu:
                if (isRemoteUp(command)) {
                    selected = static_cast<MenuOption>(
                        (static_cast<int>(selected) + MenuOptionCount - 1) % MenuOptionCount);
                    screenNeedsUpdate = true;
                } else if (isRemoteDown(command)) {
                    selected = static_cast<MenuOption>(
                        (static_cast<int>(selected) + 1) % MenuOptionCount);
                    screenNeedsUpdate = true;
                } else if (isRemoteEnter(command)) {
                    if (selected == MenuOption::Accuracy) {
                        practice.start();
                        state = AppState::Practice;
                    } else {
                        state = AppState::Instructions;
                    }
                    screenNeedsUpdate = true;
                } else if (command == RemoteLeft || command == RemoteRight) {
                    // Horizontal navigation is intentionally disabled.
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
                if (isRemoteEnter(command)) {
                    if (selected == MenuOption::Practice) {
                        practice.start();
                        state = AppState::Practice;
                    } else {
                        state = AppState::Menu;
                    }
                    screenNeedsUpdate = true;
                }
                break;

            case AppState::Practice:
                if (isRemoteExit(command)) {
                    state = AppState::Menu;
                    screenNeedsUpdate = true;
                } else {
                    practice.update(deltaTime);
                    practice.render();
                    if (practice.isFinished()) {
                        practice.start();
                    }
                }
                break;
        }

        // Drawing and DMA transfer time are included in the 30 FPS budget.
        sleep_until(from_us_since_boot(currentTime + FrameDurationUs));
    }
}
