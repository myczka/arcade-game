#include "input_manager.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"

InputManager::InputManager() {
    buttons_.at(static_cast<size_t>(Color::Red)) = Button{RED_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Green)) = Button{GREEN_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Blue)) = Button{BLUE_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::White)) = Button{WHITE_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Yellow)) = Button{YELLOW_BTN_GPIO};
    this->init();
}

void InputManager::init() {
    for (auto &b : buttons_)
    {
        gpio_init(b.pin);
        gpio_set_dir(b.pin, GPIO_IN);
        gpio_pull_up(b.pin); // or pull_down depending your wiring
        b.previous = gpio_get(b.pin);
        b.current = b.previous;
    }

    ir_->init();
}

void InputManager::update() {

}

bool InputManager::wasPressed(Color color) const {
    return buttons_.at(static_cast<size_t>(color)).previous;

}

bool InputManager::wasReleased(Color color) const {

}

bool InputManager::isHeld(Color color) const {

}

bool InputManager::wasRemotePressed() const {

}