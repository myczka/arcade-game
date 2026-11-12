#pragma once

#include "engine/output/display_manager.h"
#include "engine/input/button.h"

class Accuracy {
public:
    void render(DisplayManager& display, Color color,
                           unsigned score, unsigned secondsLeft);
    void showResults(DisplayManager& display, unsigned points,
                            unsigned incorrect, unsigned missed, unsigned correct);
};
