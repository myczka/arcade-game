#include "input_manager.h"

#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

namespace {
constexpr int64_t SampleIntervalUs = 1000;
constexpr uint32_t DebounceUs = 5000;
}

InputManager::InputManager() {
    buttons_.at(static_cast<size_t>(Color::Red)) = Button{RED_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Green)) = Button{GREEN_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Blue)) = Button{BLUE_BTN_GPIO};
    buttons_.at(static_cast<size_t>(Color::Yellow)) = Button{YELLOW_BTN_GPIO};
}

InputManager::~InputManager() {
    if (samplingStarted_) {
        cancel_repeating_timer(&sampleTimer_);
    }
}

void InputManager::init() {
    if (samplingStarted_) {
        return;
    }
    for (unsigned i = 0; i < buttons_.size(); ++i) {
        Button& b = buttons_[i];
        gpio_init(b.pin);
        gpio_set_dir(b.pin, GPIO_IN);
        gpio_pull_up(b.pin); // or pull_down depending your wiring
        b.previous = !gpio_get(b.pin);
        b.current = b.previous;
        b.candidate = b.current;
        b.candidateSinceUs = time_us_32();
        if (b.current) {
            heldMask_ |= 1u << i;
        }
    }

    ir_->init();
    samplingStarted_ = add_repeating_timer_us(-SampleIntervalUs, sampleTimer, this, &sampleTimer_);
    if (!samplingStarted_) {
        panic("Unable to start button sampling timer");
    }
}

bool InputManager::sampleTimer(repeating_timer_t* timer) {
    static_cast<InputManager*>(timer->user_data)->sampleButtons();
    return true;
}

void InputManager::sampleButtons() {
    const uint32_t now = time_us_32();
    for (unsigned i = 0; i < buttons_.size(); ++i) {
        Button& b = buttons_[i];
        const bool pressed = !gpio_get(b.pin);
        if (pressed != b.candidate) {
            b.candidate = pressed;
            b.candidateSinceUs = now;
        }

        const uint32_t mask = 1u << i;
        const bool held = (heldMask_ & mask) != 0;
        if (pressed != held && now - b.candidateSinceUs >= DebounceUs) {
            if (pressed) {
                heldMask_ |= mask;
                pendingPresses_ |= mask;
            } else {
                heldMask_ &= ~mask;
                pendingReleases_ |= mask;
            }
        }
    }
}

void InputManager::update() {
    // Snapshot events atomically with respect to the timer on this core.
    const uint32_t interruptState = save_and_disable_interrupts();
    const uint32_t held = heldMask_;
    framePresses_ = pendingPresses_;
    frameReleases_ = pendingReleases_;
    pendingPresses_ = 0;
    pendingReleases_ = 0;
    restore_interrupts(interruptState);

    for (unsigned i = 0; i < buttons_.size(); ++i) {
        Button& b = buttons_[i];
        b.previous = b.current;
        b.current = (held & (1u << i)) != 0;
    }

    remote_pressed_ = ir_->receivedCommand();
}

bool InputManager::wasPressed(Color color) const {
    return (framePresses_ & (1u << static_cast<unsigned>(color))) != 0;
}

bool InputManager::wasReleased(Color color) const {
    return (frameReleases_ & (1u << static_cast<unsigned>(color))) != 0;
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
