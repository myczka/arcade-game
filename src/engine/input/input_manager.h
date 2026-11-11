#pragma once

#include <array>

#include "button.h"
#include "ir_remote.h"

class InputManager {
    public:
        InputManager(/*std::vector<Button> buttons, IRRemote ir*/);

        void init();
        void update();
        
        // Prob only need wasPressed
        bool wasPressed(Color color) const;
        bool isHeld(Color color) const;
        bool wasReleased(Color color) const;

        bool wasRemotePressed() const;
    private:
        // buttons vec should be defined in the engine because hardware inputs are hard-implemented?
        std::array<Button, 5> buttons_ = {};
        IRRemote* ir_;

};