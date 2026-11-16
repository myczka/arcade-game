#include <cstdint>

#include "engine/input/input_manager.h"
#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"
#include "game/practice.h"
#include "pico/stdlib.h"

constexpr unsigned int BuzzerPin = 8;

int main() {
    stdio_init_all();

    InputManager input;
    DisplayManager display;
    Buzzer buzzer(BuzzerPin);
    Practice practice(input, display, buzzer);

    input.init();
    buzzer.init();
    display.init();
    practice.start();

    while (true) {
        static std::uint64_t previousTime = time_us_64();
        const std::uint64_t currentTime = time_us_64();
        const float deltaTime =
            static_cast<float>(currentTime - previousTime) / 1000000.0f;
        previousTime = currentTime;

        input.update();
        practice.update(deltaTime);
        buzzer.update();
        practice.render();

        if (practice.isFinished()) {
            practice.start();
        }

        sleep_ms(16);
    }
}
