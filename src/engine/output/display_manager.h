#pragma once

#include <stdint.h> // Provide uint16_t for TFT color values.
#include "../input/button.h" 
#include "screen.h"

class DisplayManager{
public:
    void init();
    void clear(std::uint16_t color = Screen::Black);
    void present(std::uint8_t x = 0, std::uint8_t y = 0,
                 std::uint8_t width = Screen::Width,
                 std::uint8_t height = Screen::Height);
    void drawRectangle(std::uint8_t x, std::uint8_t y, std::uint8_t width,
                       std::uint8_t height, std::uint16_t color);
    void drawSprite(std::int16_t x, std::int16_t y, const std::uint16_t* pixels,
                    std::uint8_t width, std::uint8_t height,
                    std::uint16_t transparent);
    void drawText(std::uint8_t x, std::uint8_t y, const char* text,
                  std::uint16_t color = Screen::White, std::uint8_t scale = 1,
                  std::uint16_t background = Screen::Black);

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
    // Display "Get ready" and countdown before the reaction target appears.
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
    uint16_t getTftColor(Color color);
    Screen screen_;
};
