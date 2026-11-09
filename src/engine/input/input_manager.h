#pragma once

#include <vector>

#include "hardware/gpio.h"
#include "button.h"

class InputManager {
    public:
        InputManager();

        void init();
        void update();
        
        // Prob only need wasPressed
        bool wasPressed(Color color) const;
        bool isHeld(Color color) const;
        bool wasReleased(Color color) const;


    private:
        std::vector<Button> buttons_;
        IRReceiver ir_;

};