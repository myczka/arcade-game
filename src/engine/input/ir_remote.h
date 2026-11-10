#pragma once
#include <cstdint>
#include <pico/types.h>

using namespace std;

class IRRemote {
public:
    explicit IRRemote(std::uint8_t pin);    // Constructor for the IR remote
    
    void init();                   // Initialize the IR remote
    int  receiveCommand();         // Receive an IR command
    int  getLastReceivedCommand() const;

private:
    // Used for handling GPIO events
    static void gpioCallback(uint gpio, uint32_t events);
    void handleEvent(uint32_t events);

    // Used for storing the pin number and tracking IR frame data
    uint8_t pin;    
    volatile bool frame_ready;
    volatile bool last_edge_time;

    // Used for storing the pulse durations and count for the current IR frame
    volatile uint32_t pulse_durations[100];
    volatile uint8_t pulse_count;

    // Used for storing the last received IR command
    int  last_received_command;

    // Used for decoding the current IR frame into a command
    int decodeFrame();
};