#pragma once

#include "engine/output/display_manager.h"

class Reaction {
public:
    void showWaiting(DisplayManager& display);
    void showGo(DisplayManager& display);
    void showFalseStart(DisplayManager& display);
    void showResult(DisplayManager& display, unsigned milliseconds);
};
