#include "ir_remote.h"
#include "hardware/gpio.h"
#include "pico/time.h"

namespace {
// NEC timing values, expressed in microseconds.
constexpr uint32_t NEC_FRAME_GAP_US = 10000;
constexpr uint32_t NEC_LEADER_LOW_US = 9000;
constexpr uint32_t NEC_LEADER_HIGH_US = 4500;
constexpr uint32_t NEC_BIT_LOW_US = 560;
constexpr uint32_t NEC_ZERO_HIGH_US = 560;
constexpr uint32_t NEC_ONE_HIGH_US = 1690;
constexpr uint32_t NEC_REPEAT_HIGH_US = 2250;
constexpr uint32_t NEC_TOLERANCE_US = 250;
constexpr uint8_t NEC_FRAME_PULSES = 66;
constexpr uint8_t NEC_REPEAT_PULSES = 3;
constexpr uint32_t IR_COMMAND_COOLDOWN_US = 150000;

bool within(uint32_t value, uint32_t expected, uint32_t tolerance = NEC_TOLERANCE_US) {
    // Accept timing variation caused by the remote and receiver hardware.
    const uint32_t minimum =
        expected > tolerance ? expected - tolerance : 0;
    return value >= minimum && value <= expected + tolerance;
}
}

// The GPIO callback can access only one active IRRemote instance.
static IRRemote* activeRemote = nullptr;

// Store the pin and reset the receiver state before initialization.
IRRemote::IRRemote(uint8_t pin)
    : pin(pin),
      frame_ready(false),
      capturing_frame(false),
      last_edge_time(0),
      pulse_count(0),
      last_received_command(-1),
      last_command_time_us(0),
      has_received_command(false) {
    activeRemote = this;
}

    /* PUBLIC FUNCTIONS */
// Configure the receiver input and listen for both signal edges.
void IRRemote::init() {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_disable_pulls(pin);

    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &IRRemote::gpioCallback);
}

// Decode a completed frame when the main program asks for a command.
bool IRRemote::receivedCommand()
{
    if (!this->frame_ready)
    {
        return false;
    }

    int command = decodeFrame();

    this->frame_ready = false;
    this->capturing_frame = false;
    this->pulse_count = 0;
    if (command >= 0)
    {
        const uint32_t now = time_us_32();
        if (has_received_command &&
            command == last_received_command &&
            now - last_command_time_us < IR_COMMAND_COOLDOWN_US) {
            return false;
        }

        this->last_received_command = command;
        this->last_command_time_us = now;
        this->has_received_command = true;
    }

    return command >= 0;
}

// Return the last valid command received from the remote.
int8_t IRRemote::getLastReceivedCommand() const {
    return this->last_received_command;
}

    /* PRIVATE FUNCTIONS*/
// Forward the GPIO interrupt to the active IRRemote object.
void IRRemote::gpioCallback(unsigned int gpio, uint32_t events) {
    if (activeRemote != nullptr && gpio == activeRemote->pin) {
        activeRemote->handleEvent(events);
    }
}

// Capture the duration between edges without decoding inside the interrupt.
void IRRemote::handleEvent(uint32_t) {
    if (frame_ready) {
        return;
    }

    const uint32_t now = time_us_32();
    const uint32_t duration = now - last_edge_time;

    last_edge_time = now;

    if (duration > NEC_FRAME_GAP_US) {
        capturing_frame = true;
        pulse_count = 0;
        return;
    }

    if (!capturing_frame) {
        return;
    }

    if (pulse_count < 100) {
        pulse_durations[pulse_count] = duration;
        pulse_count++;

        const bool repeat_frame =
            pulse_count == NEC_REPEAT_PULSES &&
            within(pulse_durations[0], NEC_LEADER_LOW_US, 1000) &&
            within(pulse_durations[1], NEC_REPEAT_HIGH_US, 500) &&
            within(pulse_durations[2], NEC_BIT_LOW_US);
        if (pulse_count == NEC_FRAME_PULSES || repeat_frame) {
            frame_ready = true;
            capturing_frame = false;
        }
    }
}



// Validate NEC timings and extract the command byte from the frame.
int IRRemote::decodeFrame() {
    if (pulse_count >= 3 &&
        within(pulse_durations[0], NEC_LEADER_LOW_US) &&
        within(pulse_durations[1], NEC_REPEAT_HIGH_US) &&
        within(pulse_durations[2], NEC_BIT_LOW_US)) {
        return last_received_command;
    }

    if (pulse_count < NEC_FRAME_PULSES ||
        !within(pulse_durations[0], NEC_LEADER_LOW_US, 1000) ||
        !within(pulse_durations[1], NEC_LEADER_HIGH_US, 600)) {
        return -1;
    }

    // NEC transmits 32 bits: address, inverted address, command,
    // and inverted command.
    uint32_t data = 0;
    for (uint8_t bit = 0; bit < 32; bit++) {
        const uint32_t low = pulse_durations[2 + bit * 2];
        const uint32_t high = pulse_durations[3 + bit * 2];

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
    const uint8_t command = (data >> 16) & 0xff;
    const uint8_t inverse_command = (data >> 24) & 0xff;

    if ((command ^ inverse_command) != 0xff) {
        return -1;
    }

    return command;
}
