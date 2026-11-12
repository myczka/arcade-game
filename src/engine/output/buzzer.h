#pragma once

#include <cstdint>

class Buzzer {
public:
    // Constructor
    explicit Buzzer(unsigned int pin);

    // Initialize the buzzer with the specified pin, slice, and channel
    void init();

    // Functions to control the buzzer
    // Duration is measured in milliseconds.
    void play(std::uint32_t frequency, std::uint32_t duration);
    void stop();
    void update();


private:
    // Used to initilize the pin for the buzzer
    // The pin is set in the constructor and cannot be changed after initialization.
    unsigned int pin;
    unsigned int slice;
    unsigned int channel;

    // Indicates if the buzzer is currently playing and for how long
    bool     is_playing;
    std::uint64_t stop_time;

};