#include "swimmy_fish.h"
#include "display_helpers.h"

void SwimmyFish::render(DisplayManager& display, Color color, unsigned score, unsigned secondsLeft) {
    const char* name;
    std::uint16_t rgb;
    switch (color) {
        case Color::Red: name = "RED"; rgb = 0xf800; break;
        case Color::Green: name = "GREEN"; rgb = 0x07e0; break;
        case Color::Blue: name = "BLUE"; rgb = 0x001f; break;
        case Color::Yellow: name = "YELLOW"; rgb = 0xffe0; break;
        case Color::White: name = "WHITE"; rgb = 0xffff; break;
        default: GameDisplay::message(display, "Input", "Invalid color"); return;
    }
    GameDisplay::heading(display, "PRESS THE COLOR");
    display.drawRectangle(24, 36, 80, 48, rgb);
    display.drawText(4, 96, name, Screen::White, 2);
    GameDisplay::number(display, 124, "Score: ", score);
    GameDisplay::number(display, 144, "Seconds: ", secondsLeft);
}

void SwimmyFish::showResults(DisplayManager& display, unsigned points, unsigned incorrect,
                                         unsigned missed, unsigned correct) {
    GameDisplay::heading(display, "ACCURACY RESULTS");
    GameDisplay::number(display, 40, "Points: ", points);
    GameDisplay::number(display, 60, "Wrong: ", incorrect);
    GameDisplay::number(display, 80, "Missed: ", missed);
    const std::uint64_t attempts = static_cast<std::uint64_t>(correct) + incorrect + missed;
    const unsigned percent = attempts == 0 ? 0 :
        static_cast<unsigned>(100ull * correct / attempts);
    GameDisplay::number(display, 100, "Accuracy %: ", percent);
    display.drawText(4, 136, "IR: replay / menu");
}
