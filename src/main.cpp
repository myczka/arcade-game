#include <cstdint>
#include <cstdio>
#include <array>

#include "engine/input/button.h"
#include "engine/input/ir_remote.h"
#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"
#include "pico/stdlib.h"

constexpr unsigned int BUZZER_PIN = 8;
constexpr std::uint32_t TEST_TONE_FREQUENCY = 250;
constexpr std::uint32_t TEST_TONE_DURATION_MS = 100;
constexpr std::uint64_t TEST_TONE_INTERVAL_US = 3000000;

int main() {
    stdio_init_all();

    Buzzer buzzer(BUZZER_PIN);
    IRRemote remote(REMOTE_GPIO);
    DisplayManager display;

    std::array<Button, static_cast<size_t>(Color::Count)> buttons = {
        Button{RED_BTN_GPIO},
        Button{GREEN_BTN_GPIO},
        Button{BLUE_BTN_GPIO},
        Button{YELLOW_BTN_GPIO},
        Button{WHITE_BTN_GPIO},
    };

    for (Button& button : buttons) {
        gpio_init(button.pin);
        gpio_set_dir(button.pin, GPIO_IN);
        gpio_pull_up(button.pin);
        button.previous = gpio_get(button.pin);
        button.current = button.previous;
    }

    buzzer.init();
    remote.init();
    display.init();

    printf("Buzzer and NEC IR test started.\n");
    printf("Buzzer: GPIO %u, IR receiver: GPIO %u\n", BUZZER_PIN, REMOTE_GPIO);
    printf("Buttons: Red GPIO %u, Green GPIO %u, Blue GPIO %u, "
           "Yellow GPIO %u, White GPIO %u\n",
           RED_BTN_GPIO,
           GREEN_BTN_GPIO,
           BLUE_BTN_GPIO,
           YELLOW_BTN_GPIO,
           WHITE_BTN_GPIO);

    buzzer.play(TEST_TONE_FREQUENCY, TEST_TONE_DURATION_MS);
    std::uint64_t nextToneTime = time_us_64() + TEST_TONE_INTERVAL_US;
    constexpr const char* buttonNames[] = {
        "Red", "Green", "Blue", "Yellow", "White"};

    while (true) {
        buzzer.update();

        for (std::size_t index = 0; index < buttons.size(); ++index) {
            Button& button = buttons[index];
            button.previous = button.current;
            button.current = !gpio_get(button.pin);

            if (button.current && !button.previous) {
                buzzer.play(TEST_TONE_FREQUENCY, TEST_TONE_DURATION_MS);
                printf("%s button pressed on GPIO %u.\n",
                       buttonNames[index],
                       button.pin);
            }
        }

        if (remote.receivedCommand()) {
            const int command = remote.getLastReceivedCommand();
            printf("NEC command received: 0x%02X (%d)\n", command, command);
        }

        const std::uint64_t now = time_us_64();
        if (now >= nextToneTime) {
            buzzer.play(TEST_TONE_FREQUENCY, TEST_TONE_DURATION_MS);
            nextToneTime = now + TEST_TONE_INTERVAL_US;
            printf("Buzzer test tone played.\n");
        }

        sleep_ms(1);
    }
}
