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
        std::array<Button, static_cast<size_t>(Color::Count)> buttons_;
        // std::array<Button, 5> buttons_ = {
        //     Button{Color::Red, RED_BTN_GPIO}, 
        //     Button{Color::Green, GREEN_BTN_GPIO},
        //     Button{Color::Blue, BLUE_BTN_GPIO},
        //     Button{Color::Yellow, YELLOW_BTN_GPIO},
        //     Button{Color::White, WHITE_BTN_GPIO},
        // };
        IRRemote* ir_ = new IRRemote(REMOTE_GPIO);

};