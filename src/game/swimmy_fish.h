#pragma once

#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"

#include "engine/input/input_manager.h"

struct Pipe {
    float x;
    float gap_y = 80;
    bool scored = false;
};

class SwimmyFish {
    public:
        static constexpr float GRAVITY = 300;
        static constexpr uint16_t BACKGROUND = 0x421F;

        SwimmyFish(InputManager & input, DisplayManager &display, Buzzer &buzzer);

        void start();
        void update(float dt);
        void render();

        bool isFinished() const;

    private:
        void fishLocation();
        void updateSwimSound();
        bool hasCollision() const;
        void resetPipe(Pipe& pipe, float x);
        void updatePipes(float dt);
        void drawPipes();
        void presentPipeRegion(const Pipe& previous, const Pipe& current);

        static constexpr uint8_t PIPE_WIDTH = 18;
        static constexpr uint8_t GAP_HEIGHT = 77;
        static constexpr uint8_t PIPE_MARGIN = 12;
        static constexpr float PIPE_SPACING = 96.0f;
        static constexpr float PIPE_SPEED = 40.0f;
        static constexpr uint16_t PIPE_COLOR = 0x07E0;

        InputManager& input_;
        DisplayManager& display_;
        Buzzer& buzzer_;

        //const float fish_start_x = 48;
        const float fish_start_y = 64;

        const float fish_x = 48;
        float fish_y;
        float fish_velocity = 0;
        uint8_t transition_frames_ = 0;
        int16_t displayed_fish_y_ = 64;


        const float fish_cntr_x = fish_x + 16;
        float fish_cntr_y = fish_start_y + 16;

        const float fish_left = fish_cntr_x - 11;
        const float fish_right = fish_cntr_x + 7;
        float fish_top;
        float fish_bottom;

        struct Pipe pipes[2];
        Pipe displayed_pipes[2]{};
        bool full_present_needed = true;

        bool running_ = false;
        uint8_t sound_frames_ = 0;
        uint8_t sound_volume_ = 255;
        bool sound_c3_ = true;
        Color color_state_ = Color::Blue;






};
