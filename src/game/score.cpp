#include "score.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

Score::Score(DisplayManager& display) : display_(display) {}

void Score::init(bool accuracy) {
    score_ = 0;
    accuracy_ = accuracy;
}

void Score::update(unsigned score) {
    if (accuracy_) {
        score_ = std::min(score, 100u);
    } 
    else {
        score_ = score;
    }
}

unsigned Score::value() const {
    return score_;
}

void Score::render(uint16_t background) {
    char text[12];

    if (accuracy_) {
        std::snprintf(text, sizeof(text), "%u%%", score_);
    } 
    else {
        std::snprintf(text, sizeof(text), "%u", score_);
    }

    if (background != DisplayManager::Transparent) {
        display_.drawRectangle(X, Y, Width, Height, background);
    }

    const auto x = static_cast<uint8_t>(DisplayManager::Width - 4 - std::strlen(text) * 6);

    display_.drawText(x, Y, text, DisplayManager::White, 1, background);
}
