#include <algorithm>
#include <stdint.h>

#include "engine/input/input_manager.h"
#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"
#include "game/game_manager.h"
#include "pico/stdlib.h"

constexpr unsigned int BuzzerPin = 8;
constexpr uint64_t FrameDurationUs = 1000000 / 30;

int main() {
    stdio_init_all();

    InputManager input;
    DisplayManager display;
    Buzzer buzzer(BuzzerPin);
    GameManager games(input, display, buzzer);

    input.init();
    buzzer.init();
    display.init();
    games.init();

    uint64_t previousTime = time_us_64();
    while (true) {
        const uint64_t currentTime = time_us_64();
        const float deltaTime =
            static_cast<float>(currentTime - previousTime) / 1000000.0f;
        previousTime = currentTime;

        input.update();
        buzzer.update();
        games.update(deltaTime);
        games.render();

        // Drawing and DMA transfer time are included in the 30 FPS budget.
        const uint64_t frameEnd = currentTime + FrameDurationUs;
        while (time_us_64() < frameEnd) {
            buzzer.update();
            const uint64_t nextAudioUpdate = time_us_64() + 2000;
            sleep_until(from_us_since_boot(std::min(frameEnd, nextAudioUpdate)));
        }
    }
}
