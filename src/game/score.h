#pragma once

#include "engine/output/display_manager.h"

// Live score in the upper-right corner. Games present it with their scene.
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

private:
    DisplayManager& display_;
    unsigned score_ = 0;
    bool accuracy_ = false;
};
