#pragma once

#include <cstdint>

#include "../input/button.h"

// Game-facing drawing interface for the buffered 128x160 ST7735 display.
class DisplayManager {
public:
    static constexpr std::uint8_t Width = 128;
    static constexpr std::uint8_t Height = 160;
    static constexpr std::uint16_t Black = 0x0000;
    static constexpr std::uint16_t White = 0xffff;

    void init();
    void clear(std::uint16_t color = Black);
    // Primitive drawing changes RAM only. Present after composing the frame.
    void present(std::int16_t x = 0, std::int16_t y = 0,
                 std::uint8_t width = Width,
                 std::uint8_t height = Height);
    void drawRectangle(std::int16_t x, std::int16_t y, std::uint8_t width,
                       std::uint8_t height, std::uint16_t color);
    void drawSprite(std::int16_t x, std::int16_t y, const std::uint16_t* pixels,
                    std::uint8_t width, std::uint8_t height,
                    std::uint16_t transparent);
    void drawText(std::uint8_t x, std::uint8_t y, const char* text,
                  std::uint16_t color = White, std::uint8_t scale = 1,
                  std::uint16_t background = Black);

    // Display the welcome screen and game title
    void showTitle();

    // Display the menu and highlight the selected game
    // 0 = Accuracy, 1 = Reaction, 2 = Memory.
    void showMenu(int selected);

    // Display instructions from "int selected" and "Press ENTER to start"
    // mode uses the same numbering as showMenu().
    void showInstructions(int mode);

    //ACCURACY GAME SCREEN
    // Display the target button color and remaining accuracy-session time.
    void showAccuracyPrompt(Color target, unsigned secondsLeft);

    // Display the final accuracy statistics supplied by the game.
    void showAccuracyResults(unsigned correct, unsigned missed, int score, unsigned accuracyPercent);

    //REACTION GAME SCREEN
    // Display "Get ready" before the reaction target appears; game code owns timing.
    void showReactionReady();

    // Reveal the button color the player should press.
    // Edge case: Game code ignores wrong presses and keeps the timer running.
    void showReactionPrompt(Color target);

    // Display the reaction time after the correct button is pressed.
    void showReactionResults(float milliseconds);

    //MEMORY GAME SCREEN
    // Display one color from the memory sequence, round, and remaining time.
    void showMemoryColor(Color color, unsigned round, unsigned secondsLeft);

    // Remove the displayed color between sequence steps.
    // Keep the round and remaining time visible.
    void clearMemoryColor();

    // Hide the playback color and display "Your turn", round, and time.
    void showMemoryTurn(unsigned round, unsigned secondsLeft);

    // Display memory results, including the longest completed sequence.
    void showMemoryResults(unsigned correct, unsigned missed, int score, unsigned longestSequence);

    // Add Play again / Exit options to any game's results screen.
    // 0 = Play again selected, 1 = Exit selected.
    void showResultOptions(int selected);

private:
    // Convert Color::Blue, for example, into ST7735_BLUE for drawing.
    std::uint16_t getTftColor(Color color);
    void heading(const char* title);
    void statistic(std::uint8_t y, const char* label, unsigned value);
};
