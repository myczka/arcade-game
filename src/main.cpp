#include <cstdint>

#include "engine/input/input_manager.h"
#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"
#include "game/practice.h"
#include "pico/stdlib.h"

constexpr unsigned int BuzzerPin = 8;
constexpr std::uint64_t FrameDurationUs = 1000000 / 30;

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

    std::uint64_t previousTime = time_us_64();
    while (true) {
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

        // Drawing and DMA transfer time are included in the 30 FPS budget.
        sleep_until(from_us_since_boot(currentTime + FrameDurationUs));
    }
}
