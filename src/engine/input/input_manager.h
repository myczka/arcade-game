#pragma once

#include <array>

#include "button.h"
#include "ir_remote.h"

class InputManager {
    public:
        InputManager(/*std::vector<Button> buttons, IRRemote ir*/);

        void init();
        void update();
        
        // Buttons
        bool wasPressed(Color color) const;
        bool isHeld(Color color) const;
        bool wasReleased(Color color) const;

        // Remote
        int getRemoteCommand() const;
        int getLastRemoteCommand() const;

    private:
        // buttons vec should be defined in the engine because hardware inputs are hard-implemented?
        std::array<Button, static_cast<size_t>(Color::Count)> buttons_;

        IRRemote* ir_ = new IRRemote(REMOTE_GPIO);

        bool remote_pressed_ = false;

};