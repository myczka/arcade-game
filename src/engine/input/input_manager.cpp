#include "input_manager.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"

InputManager::InputManager() {



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

}

bool InputManager::wasReleased(Color color) const {

}

bool InputManager::isHeld(Color color) const {

}

bool InputManager::wasRemotePressed() const {

}