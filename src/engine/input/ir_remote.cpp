#include "ir_remote.h"
#include "hardware/gpio.h"

// Constructor
IRRemote::IRRemote(uint8_t pin) : pin(pin), last_received_command(-1) {}

// Hardware setup
void IRRemote::init() {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_disable_pulls(pin);
}

// Read and decode an IR signal
int IRRemote::receiveCommand() {
    // Wait for the IR signal
    while (gpio_get(pin) == 0) {
        // Wait for the IR signal to start
    }
    
    // Measure the pulse durations

    // Decode the remote protocol

    // Store the decoded command
    
    // Return the command
    return last_received_command;
}

// Return the most recently received command
int IRRemote::getLastReceivedCommand() const {
    return last_received_command;
}
