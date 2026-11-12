#include "ir_remote.h"
#include "hardware/gpio.h"
#include "pico/time.h"

namespace {
// NEC timing values, expressed in microseconds.
constexpr std::uint32_t NEC_FRAME_GAP_US = 10000;
constexpr std::uint32_t NEC_LEADER_LOW_US = 9000;
constexpr std::uint32_t NEC_LEADER_HIGH_US = 4500;
constexpr std::uint32_t NEC_BIT_LOW_US = 560;
constexpr std::uint32_t NEC_ZERO_HIGH_US = 560;
constexpr std::uint32_t NEC_ONE_HIGH_US = 1690;
constexpr std::uint32_t NEC_REPEAT_HIGH_US = 2250;
constexpr std::uint32_t NEC_TOLERANCE_US = 250;

bool within(std::uint32_t value, std::uint32_t expected) {
    // Accept timing variation caused by the remote and receiver hardware.
    return value >= expected - NEC_TOLERANCE_US &&
           value <= expected + NEC_TOLERANCE_US;
}
}

// The GPIO callback can access only one active IRRemote instance.
static IRRemote* activeRemote = nullptr;

// Store the pin and reset the receiver state before initialization.
IRRemote::IRRemote(std::uint8_t pin)
    : pin(pin),
      frame_ready(false),
      last_edge_time(0),
      pulse_count(0),
      last_received_command(-1) {
    activeRemote = this;
}

// Configure the receiver input and listen for both signal edges.
void IRRemote::init() {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_disable_pulls(pin);

    gpio_set_irq_enabled_with_callback(
        pin,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &IRRemote::gpioCallback);
}

// Forward the GPIO interrupt to the active IRRemote object.
void IRRemote::gpioCallback(uint32_t gpio, std::uint32_t events) {
    if (activeRemote != nullptr && gpio == activeRemote->pin) {
        activeRemote->handleEvent(events);
    }
}

// Capture the duration between edges without decoding inside the interrupt.
void IRRemote::handleEvent(std::uint32_t) {
    if (frame_ready) {
        return;
    }

    const std::uint32_t now = time_us_32();
    const std::uint32_t duration = now - last_edge_time;

    last_edge_time = now;

    if (duration > NEC_FRAME_GAP_US) {
        if (pulse_count > 0) {
            frame_ready = true;
        }
        return;
    }

    if (pulse_count < 100) {
        pulse_durations[pulse_count] = duration;
        pulse_count++;
    }
}

// Decode a completed frame when the main program asks for a command.
int IRRemote::receiveCommand() {
    if (!frame_ready) {
        return -1;
    }

    int command = decodeFrame();

    frame_ready = false;
    pulse_count = 0;
    if (command >= 0) {
        last_received_command = command;
    }

    return command;
}

// Return the last valid command received from the remote.
int IRRemote::getLastReceivedCommand() const {
    return last_received_command;
}

// Validate NEC timings and extract the command byte from the frame.
int IRRemote::decodeFrame() {
    if (pulse_count >= 3 &&
        within(pulse_durations[0], NEC_LEADER_LOW_US) &&
        within(pulse_durations[1], NEC_REPEAT_HIGH_US) &&
        within(pulse_durations[2], NEC_BIT_LOW_US)) {
        return last_received_command;
    }

    if (pulse_count < 66 ||
        !within(pulse_durations[0], NEC_LEADER_LOW_US) ||
        !within(pulse_durations[1], NEC_LEADER_HIGH_US)) {
        return -1;
    }

    // NEC transmits 32 bits: address, inverted address, command,
    // and inverted command.
    std::uint32_t data = 0;
    for (std::uint8_t bit = 0; bit < 32; bit++) {
        const std::uint32_t low = pulse_durations[2 + bit * 2];
        const std::uint32_t high = pulse_durations[3 + bit * 2];

        if (!within(low, NEC_BIT_LOW_US)) {
            return -1;
        }

        // A longer high pulse represents one; a shorter pulse represents zero.
        data >>= 1;
        if (within(high, NEC_ONE_HIGH_US)) {
            data |= 0x80000000u;
        } else if (!within(high, NEC_ZERO_HIGH_US)) {
            return -1;
        }
    }

    // The command is followed by its bitwise inverse for validation.
    const std::uint8_t command = (data >> 16) & 0xff;
    const std::uint8_t inverse_command = (data >> 24) & 0xff;

    if ((command ^ inverse_command) != 0xff) {
        return -1;
    }

    return command;
}
