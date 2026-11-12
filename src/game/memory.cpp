#include "memory.h"
#include "display_helpers.h"

void Memory::showWatch(DisplayManager& display) {
    GameDisplay::message(display, "MEMORY", "Watch the sequence");
}

void Memory::showYourTurn(DisplayManager& display) {
    GameDisplay::message(display, "MEMORY", "Repeat the sequence");
}

void Memory::showResult(DisplayManager& display, unsigned longestSequence) {
    GameDisplay::result(display, "MEMORY RESULT", longestSequence, "Longest sequence");
}
