#pragma once


#include <cstdint>
#include <pico/types.h>

class IRRemote {
public:
    // Creates an IR receiver attached to the specified GPIO pin.
    explicit IRRemote(std::uint8_t pin);

    // Configures the GPIO pin and enables edge-triggered interrupts.
    void init();

    // Returns a newly decoded command, or -1 when no command is ready.
    int  receiveCommand();

    // Returns the most recently decoded command.
    int  getLastReceivedCommand() const;

private:
    // Static adapter called by the Pico GPIO interrupt system.
    static void gpioCallback(std::uint gpio, std::uint32_t events);

    // Records the time between signal edges while an NEC frame arrives.
    void handleEvent(std::uint32_t events);

    // Converts the captured NEC pulse timings into a command byte.
    int decodeFrame();

    // GPIO pin connected to the IR receiver module.
    std::uint8_t pin;

    // Set by the interrupt handler when a complete frame is available.
    volatile bool frame_ready;

    // Timestamp of the previous GPIO edge, in microseconds.
    volatile std::uint32_t last_edge_time;

    // Pulse durations captured from the current NEC frame.
    volatile std::uint32_t pulse_durations[100];

    // Number of valid entries currently in pulse_durations.
    volatile std::uint8_t pulse_count;

    // Last valid command; -1 means no command has been decoded.
    int last_received_command;
};
