#pragma once
#include <cstdint>

class IRRemote {
public:
    explicit IRRemote(std::uint8_t pin);    // Constructor for the IR remote
    
    void init();                   // Initialize the IR remote
    int  receiveCommand();         // Receive an IR command

private:
    std::uint8_t pin;              // Pin number for the IR remote
    int  last_received_command;    // Store the last received IR command

    int  getLastReceivedCommand(); // Get the last received IR command

};