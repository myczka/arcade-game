#pragma once

#include <vector>

#include "hardware/gpio.h"
#include "button.h"

class InputManager {
    public:
        InputManager();

        void init();
        void update();
        
        bool wasPressed(Color color);
        bool isPressed(Color color);


    private:
        std::vector<Button> buttons_;
        IRReceiver ir_;

};