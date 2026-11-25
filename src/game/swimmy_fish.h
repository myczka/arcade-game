#pragma once

#include "engine/input/input_manager.h"
#include "engine/output/display_manager.h"
#include "engine/output/buzzer.h"
#include "score.h"

struct Pipe {
    float x;
    float gap_y = 80;
    bool scored = false;
};

struct Fish {
    const float start_y = 64;
    const float x = 55;
    const float center_x = x + 16;
    const float left = center_x - 11;
    const float right = center_x + 7;

    float y = start_y;
    float center_y = start_y + 16;
    float top = center_y - 5;
    float bottom = center_y + 5;

    int16_t displayed_y = 64;

    uint8_t transition_frames = 0;
    float velocity = 0;
    Color color = Color::Blue;
};

class SwimmyFish {
    public:
        SwimmyFish(InputManager& input, DisplayManager& display, Buzzer& buzzer, Score& score);

        void start();
        void update(float dt);
        void render();
        void renderGameOver();

        bool isFinished() const;

    private:
        // Helper functions
        void fishLocation();
        bool hasCollision() const;
        void resetPipe(Pipe& pipe, float x);
        void updatePipes(float dt);
        int16_t drawScene(bool dimmed);
        void drawPipes(uint16_t color);
        void presentPipeRegion(const Pipe& previous, const Pipe& current);

        // General constants
        static constexpr float GRAVITY = 250;
        static constexpr uint16_t BACKGROUND = 0x409a;
        static constexpr float JUMP_SPEED = -130;

        // Pipe constants
        static constexpr uint8_t PIPE_WIDTH = 18;
        static constexpr uint8_t GAP_HEIGHT = 77;
        static constexpr uint8_t PIPE_MARGIN = 12;
        static constexpr float PIPE_SPACING = 96.0f;
        static constexpr float PIPE_SPEED = 40.0f;
        static constexpr uint16_t PIPE_COLOR = 0xF206;

        // 1 second to process initial fish color
        uint8_t ready_frames_ = 30;

        // Is game running?
        bool running_ = false;

        // Dependecies
        InputManager& input_;
        DisplayManager& display_;
        Buzzer& buzzer_;
        Score& score_;

        // Objects
        Fish fish;
        Pipe pipes[2];
        Pipe prev_pipes[2]{};
};
