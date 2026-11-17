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

void Score::finish() {
    if (accuracy_) {
        practice_best_ = std::max(practice_best_, score_);
        practice_attempted_ = true;
    } else {
        swimmy_best_ = std::max(swimmy_best_, score_);
        swimmy_attempted_ = true;
    }
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

void Score::renderResults() {
    constexpr uint16_t Background = 0x5B7C;

    display_.clear(Background);
    display_.drawText(43, 20, "RESULTS", DisplayManager::White, 1, Background);

    char text[32];
    if (swimmy_attempted_) {
        std::snprintf(text, sizeof(text), "BEST FISH: %u", swimmy_best_);
    } else {
        std::snprintf(text, sizeof(text), "BEST FISH: ?");
    }
    display_.drawText(4, 65, text, DisplayManager::White, 1, Background);

    if (practice_attempted_) {
        std::snprintf(text, sizeof(text), "BEST PRACTICE: %u%%", practice_best_);
    } else {
        std::snprintf(text, sizeof(text), "BEST PRACTICE: ?");
    }
    display_.drawText(4, 90, text, DisplayManager::White, 1, Background);
    display_.present();
}
