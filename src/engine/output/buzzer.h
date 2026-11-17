#pragma once

#include <stdint.h>

class Buzzer {
public:
    // Constructor
    explicit Buzzer(unsigned int pin);

    // Initialize the buzzer with the specified pin, slice, and channel
    void init();

    // Functions to control the buzzer
    // Duration is milliseconds; volume is 0 (silent) to 255 (50% duty).
    // release_volume < 255 adds a brief ping, then a quiet fade to zero.
    void play(uint32_t frequency, uint32_t duration, uint8_t volume = 255, uint8_t release_volume = 255);
    // Sweep pitch and fade to silence. Frequencies are clamped to 20–20000 Hz.
    void sweep(uint32_t start_frequency, uint32_t end_frequency, uint32_t duration = 1000, uint8_t release_volume = 32);
    void stop();
    void update();


private:
    void setTone(uint32_t frequency, uint8_t volume, bool restart = false);
    // Used to initilize the pin for the buzzer
    // The pin is set in the constructor and cannot be changed after initialization.
    unsigned int pin;
    unsigned int slice;
    unsigned int channel;

    // Indicates if the buzzer is currently playing and for how long
    bool     is_playing;
    uint64_t stop_time;
    bool sweeping = false;
    uint64_t start_time = 0;
    uint8_t start_volume_ = 255;
    uint8_t release_volume_ = 255;
    uint32_t sweep_start_frequency = 0;
    uint32_t sweep_end_frequency = 0;

};
