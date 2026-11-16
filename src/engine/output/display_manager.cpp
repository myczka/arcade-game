#include "display_manager.h"

#include <cstdio>

#include "hardware/spi.h"
#include "pico/stdlib.h"

extern "C" {
#include "ST7735_TFT.h"
#include "hw.h"
}

void DisplayManager::heading(const char* title) {
    clear();
    drawRectWH(2, 2, 124, 156, ST7735_CYAN);
    drawText(10, 15, title, ST7735_CYAN);
    drawFastHLine(12, 32, 104, ST7735_CYAN);
}

void DisplayManager::statistic(std::uint8_t y, const char* label, unsigned value) {
    char text[40];
    std::snprintf(text, sizeof(text), "%s %u", label, value);
    drawText(10, y, text);
}

// Convert the gameplay color into its TFT drawing value.
std::uint16_t DisplayManager::getTftColor(Color color) {
    switch (color) {
        case Color::Red:
            return ST7735_RED;

        case Color::Green:
            return ST7735_GREEN;

        case Color::Blue:
            return ST7735_BLUE;

        case Color::Yellow:
            return ST7735_YELLOW;
        case Color::Count:
            break;
    }

    return ST7735_BLACK; // Fallback for an unexpected color value.
}

// Display the welcome screen with a border and centered title.
void DisplayManager::showTitle() {
    clear(); // Clear previous screen content.

    drawRectWH(2, 2, 124, 156, ST7735_CYAN); // Outer screen border.

    drawRectWH(12, 48, 104, 32, ST7735_CYAN); // Box around the title.
    drawText(37, 60, "THE CAGES", ST7735_CYAN, 1, Black);

    drawText(31, 100, "Press ENTER", ST7735_WHITE, 1, Black);
    drawText(31, 115, "to continue", ST7735_WHITE, 1, Black);
    present();
}

// Display the menu and draw a box around the selected game.
void DisplayManager::showMenu(int selected) {
    clear(); // Clear the previous menu and selection box.

    drawRectWH(2, 2, 124, 156, ST7735_CYAN); // Outer screen border.
    drawText(34, 15, "CHALLENGES", ST7735_CYAN, 1, Black);
    drawLine(12, 32, 115, 32, ST7735_CYAN); // Heading separator.

    // Highlight Accuracy when selected equals 0.
    if (selected == 0) {
        drawRectWH(12, 45, 104, 24, ST7735_YELLOW);
    }

    drawText(40, 53, "Accuracy", selected == 0 ? ST7735_YELLOW : ST7735_WHITE, 1, Black);

    // Highlight Reaction when selected equals 1.
    if (selected == 1) {
        drawRectWH(12, 75, 104, 24, ST7735_YELLOW);
    }

    drawText(40, 83, "Reaction", selected == 1 ? ST7735_YELLOW : ST7735_WHITE, 1, Black);

    // Highlight Memory when selected equals 2.
    if (selected == 2) {
        drawRectWH(12, 105, 104, 24, ST7735_YELLOW);
    }

    drawText(46, 113, "Memory", selected == 2 ? ST7735_YELLOW : ST7735_WHITE, 1, Black);

    // Display the selection instruction beneath the game choices.
    drawText(19, 140, "ENTER to select", ST7735_WHITE, 1, Black);
    present();
}

// Display instructions for the selected game and an ENTER prompt.
void DisplayManager::showInstructions(int mode) {
    clear();

    drawRectWH(2, 2, 124, 156, ST7735_CYAN); // Outer screen border.
    drawLine(12, 32, 115, 32, ST7735_CYAN); // Heading separator.

    switch (mode) {
        case 0: // Accuracy instructions.
            drawText(40, 15, "ACCURACY", ST7735_CYAN, 1, Black);
            drawText(10, 50, "Press the matching", ST7735_WHITE, 1, Black);
            drawText(19, 65, "colored button.", ST7735_WHITE, 1, Black);
            drawText(13, 90, "Get as many right", ST7735_WHITE, 1, Black);
            drawText(13, 105, "before time ends!", ST7735_WHITE, 1, Black);
            break;

        case 1: // Reaction instructions.
            drawText(40, 15, "REACTION", ST7735_CYAN, 1, Black);
            drawText(10, 55, "Wait for a circle.", ST7735_WHITE, 1, Black);
            drawText(10, 80, "Press its matching", ST7735_WHITE, 1, Black);
            drawText(19, 95, "button quickly!", ST7735_WHITE, 1, Black);
            break;

        case 2: // Memory instructions.
            drawText(46, 15, "MEMORY", ST7735_CYAN, 1, Black);
            drawText(13, 45, "Watch the colors.", ST7735_WHITE, 1, Black);
            drawText(7, 65, "Repeat the sequence", ST7735_WHITE, 1, Black);
            drawText(10, 80, "using the buttons.", ST7735_WHITE, 1, Black);
            drawText(19, 105, "Beat the timer!", ST7735_WHITE, 1, Black);
            break;
    }

    // Display the start instruction inside a yellow outline.
    drawRectWH(12, 130, 104, 22, ST7735_YELLOW);
    drawText(22, 137, "ENTER to start", ST7735_YELLOW, 1, Black);
    present();
}

void DisplayManager::showAccuracyPrompt(Color target, unsigned secondsLeft) {
    heading("ACCURACY");
    drawRectangle(24, 48, 80, 48, getTftColor(target));
    statistic(112, "Seconds:", secondsLeft);
    drawText(10, 136, "Match the color");
    present();
}

void DisplayManager::showAccuracyResults(unsigned correct, unsigned missed,
                                         int score, unsigned accuracyPercent) {
    heading("ACCURACY RESULTS");
    statistic(44, "Correct:", correct);
    statistic(63, "Missed:", missed);
    char text[40];
    std::snprintf(text, sizeof(text), "Score: %d", score);
    drawText(10, 82, text);
    std::snprintf(text, sizeof(text), "Accuracy: %u%%", accuracyPercent);
    drawText(10, 101, text);
    showResultOptions(0);
}

void DisplayManager::showReactionReady() {
    heading("REACTION");
    drawText(10, 55, "Get ready", ST7735_YELLOW, 2);
    drawText(10, 90, "Wait for a circle");
    present();
}

void DisplayManager::showReactionPrompt(Color target) {
    heading("REACTION");
    fillCircle(64, 74, 26, getTftColor(target));
    drawText(10, 115, "Match the color!");
    present();
}

void DisplayManager::showReactionResults(float milliseconds) {
    heading("REACTION RESULTS");
    char text[40];
    std::snprintf(text, sizeof(text), "Time: %.1f ms", milliseconds);
    drawText(10, 65, text);
    showResultOptions(0);
}

void DisplayManager::showMemoryColor(Color color, unsigned round,
                                    unsigned secondsLeft) {
    heading("MEMORY");
    drawText(10, 38, "Watch the colors");
    drawRectangle(24, 54, 80, 44, getTftColor(color));
    statistic(111, "Round:", round);
    statistic(132, "Seconds:", secondsLeft);
    present();
}

void DisplayManager::clearMemoryColor() {
    drawRectangle(24, 54, 80, 44, Black);
    present(24, 54, 80, 44);
}

void DisplayManager::showMemoryTurn(unsigned round, unsigned secondsLeft) {
    heading("MEMORY");
    drawText(10, 60, "Your turn", ST7735_YELLOW, 2);
    statistic(111, "Round:", round);
    statistic(132, "Seconds:", secondsLeft);
    present();
}

void DisplayManager::showMemoryResults(unsigned correct, unsigned missed,
                                      int score, unsigned longestSequence) {
    heading("MEMORY RESULTS");
    statistic(44, "Correct:", correct);
    statistic(63, "Missed:", missed);
    char text[40];
    std::snprintf(text, sizeof(text), "Score: %d", score);
    drawText(10, 82, text);
    statistic(101, "Longest:", longestSequence);
    showResultOptions(0);
}

void DisplayManager::showResultOptions(int selected) {
    // Update the options without erasing the statistics above them.
    drawRectangle(6, 126, 116, 28, Black);
    drawRectWH(6, 129, 74, 22,
               selected == 0 ? ST7735_YELLOW : ST7735_WHITE);
    drawRectWH(84, 129, 38, 22,
               selected == 1 ? ST7735_YELLOW : ST7735_WHITE);
    drawText(13, 136, "Play again",
             selected == 0 ? ST7735_YELLOW : ST7735_WHITE);
    drawText(91, 136, "Exit",
             selected == 1 ? ST7735_YELLOW : ST7735_WHITE);
    present();
}

void DisplayManager::init() {
    spi_init(SPI_TFT_PORT, 10 * 1000 * 1000);
    spi_set_format(SPI_TFT_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);
    tft_spi_init();
    TFT_BlackTab_Initialize();
    setTextWrap(false);
    clear();
    present();
}

void DisplayManager::clear(std::uint16_t color) {
    fillScreen(color);
}

void DisplayManager::present(std::int16_t x, std::int16_t y,
                             std::uint8_t width, std::uint8_t height) {
    presentRectangle(x, y, width, height);
}

void DisplayManager::drawRectangle(std::int16_t x, std::int16_t y,
                                    std::uint8_t width, std::uint8_t height,
                                    std::uint16_t color) {
    fillRectangle(x, y, width, height, color);
}

void DisplayManager::drawSprite(std::int16_t x, std::int16_t y,
                                const std::uint16_t* pixels,
                                std::uint8_t width, std::uint8_t height,
                                std::uint16_t transparent) {
    drawBitmap(x, y, pixels, width, height, transparent);
}

void DisplayManager::drawText(std::uint8_t x, std::uint8_t y, const char* text,
                              std::uint16_t color, std::uint8_t scale,
                              std::uint16_t background) {
    if (text == nullptr || scale == 0 || x >= Width || y >= Height) return;
    const unsigned cellWidth = 6u * scale;
    const unsigned cellHeight = 7u * scale;
    if (cellHeight > Height - y) return;

    unsigned cursor = x;
    while (*text != '\0' && cursor + cellWidth <= Width) {
        // The driver draws only five glyph columns; clear the spacing too.
        drawRectangle(static_cast<std::uint8_t>(cursor), y,
                      static_cast<std::uint8_t>(cellWidth),
                      static_cast<std::uint8_t>(cellHeight), background);
        drawChar(static_cast<std::uint8_t>(cursor), y,
                 static_cast<std::uint8_t>(*text++), color, background, scale);
        cursor += cellWidth;
    }
}
