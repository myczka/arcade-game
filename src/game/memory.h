#pragma once

#include "engine/output/display_manager.h"

class Memory {
public:
    void showWatch(DisplayManager& display);
    void showYourTurn(DisplayManager& display);
    void showResult(DisplayManager& display, unsigned longestSequence);
};
