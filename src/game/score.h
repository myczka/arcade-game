#pragma once

#include "engine/output/display_manager.h"

// Live score, session high scores, and the results screen.
class Score {
public:
    static constexpr uint8_t X = 64;
    static constexpr uint8_t Y = 4;
    static constexpr uint8_t Width = DisplayManager::Width - X;
    static constexpr uint8_t Height = 7;

    explicit Score(DisplayManager& display);
    void init(bool accuracy = false);
    void update(unsigned score);
    unsigned value() const;
    void render(uint16_t background = DisplayManager::Transparent);
    // Record a completed round. init() preserves both high scores.
    void finish();
    // Draw and present the title and both high scores.
    void renderResults();

private:
    DisplayManager& display_;
    unsigned score_ = 0;
    bool accuracy_ = false;
    unsigned swimmy_best_ = 0;
    unsigned practice_best_ = 0;
    bool swimmy_attempted_ = false;
    bool practice_attempted_ = false;
};
