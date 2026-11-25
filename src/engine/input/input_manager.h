#pragma once

#include <array>
#include "pico/time.h"

#include "button.h"
#include "ir_remote.h"

class InputManager {
    public:
        InputManager(/*std::vector<Button> buttons, IRRemote ir*/);
        ~InputManager();

        // The sampling timer holds this object's address.
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        InputManager(InputManager&&) = delete;
        InputManager& operator=(InputManager&&) = delete;

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
        static bool sampleTimer(repeating_timer_t* timer);
        void sampleButtons();

        // buttons vec should be defined in the engine because hardware inputs are hard-implemented?
        std::array<Button, static_cast<size_t>(Color::Count)> buttons_;
        repeating_timer_t sampleTimer_{};
        bool samplingStarted_ = false;

        // Timer-owned state, copied/cleared with interrupts disabled in update().
        volatile uint32_t heldMask_ = 0;
        volatile uint32_t pendingPresses_ = 0;
        volatile uint32_t pendingReleases_ = 0;
        uint32_t framePresses_ = 0;
        uint32_t frameReleases_ = 0;

        IRRemote* ir_ = new IRRemote(REMOTE_GPIO);

        bool remote_pressed_ = false;

};
