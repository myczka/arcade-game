#pragma once

class Buzzer {
public:
    // Constructor
    explicit Buzzer(unsigned int pin);

    // Functions to control the buzzer
    void play(uint32_t frequency, uint32_t duration);
    void stop();

private:
    // Used to initilize the pin for the buzzer
    // The pin is set in the constructor and cannot be changed after initialization.
    unsigned int pin;
    unsigned int slice = 0;

    // Indicates if the buzzer is currently playing and for how long
    bool     is_playing = false;
    uint64_t stop_time = 0;

};