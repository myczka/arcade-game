#pragma once

#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"

#include "engine/input/input_manager.h"


class SwimmyFish {
    public:
        SwimmyFish(InputManager& input, DisplayManager& display, Buzzer& buzzer);

        void start();
        void update(float dt);
        void render();

        bool isFinished() const;

    private:
        InputManager& input_;
        DisplayManager& display_;
        Buzzer& buzzer_;
        // [0] = x, [1] = y
        float fish_coord[2]{};
        float fish_velocity;
        bool running_;




};
