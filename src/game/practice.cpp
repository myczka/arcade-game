#include "practice.h"
#include "display_helpers.h"

void Reaction::showWaiting(DisplayManager& display) {
    GameDisplay::message(display, "REACTION", "Wait for green");
}

void Reaction::showGo(DisplayManager& display) {
    GameDisplay::heading(display, "REACTION");
    display.drawRectangle(24, 36, 80, 48, 0x07e0);
    display.drawText(4, 96, "GO!", DisplayManager::White, 2);
    display.present();
}

void Reaction::showFalseStart(DisplayManager& display) {
    GameDisplay::message(display, "REACTION", "False start!");
}

void Reaction::showResult(DisplayManager& display, unsigned milliseconds) {
    GameDisplay::result(display, "REACTION RESULT", milliseconds, "milliseconds");
}
