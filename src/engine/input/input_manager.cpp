#include "input_manager.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"

InputManager::InputManager() {
    buttons_.at(static_cast<size_t>(Color::Red)) = Button{RED_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Green)) = Button{GREEN_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Blue)) = Button{BLUE_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::White)) = Button{WHITE_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Yellow)) = Button{YELLOW_BTN_GPIO};
}

void InputManager::init() {
    for (auto &b : buttons_) {
        gpio_init(b.pin);
        gpio_set_dir(b.pin, GPIO_IN);
        gpio_pull_up(b.pin); // or pull_down depending your wiring
        b.previous = gpio_get(b.pin);
        b.current = b.previous;
    }

    ir_->init();
}

void InputManager::update() {
    for (auto &b : buttons_) {
        b.previous = b.current;
        b.current = !gpio_get(b.pin);
    }

    remote_pressed_ = ir_->receivedCommand();
}

bool InputManager::wasPressed(Color color) const {
    uint32_t theColor = static_cast<uint32_t>(color);
    return buttons_.at(theColor).current && !buttons_.at(theColor).previous;
}

bool InputManager::wasReleased(Color color) const {
    uint32_t theColor = static_cast<uint32_t>(color);
    return !buttons_.at(theColor).current && buttons_.at(theColor).previous;
}

bool InputManager::isHeld(Color color) const {
    uint32_t theColor = static_cast<uint32_t>(color);
    return buttons_.at(theColor).current && buttons_.at(theColor).previous;
}

int InputManager::getRemoteCommand() const {
    if (this->remote_pressed_) {
        return ir_->getLastReceivedCommand();
    }
    else {
        return -0x01;
    }
}

int InputManager::getLastRemoteCommand() const {
    return ir_->getLastReceivedCommand();
}