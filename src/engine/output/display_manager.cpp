#include "display_manager.h" 
#include "screen.h"
#include <cstdio>

// Convert the gameplay color into its TFT drawing value.
uint16_t DisplayManager::getTftColor(Color color) {
    switch (color) {
        case Color::Red:
            return ST7735_RED;

        case Color::Green:
            return ST7735_GREEN;

        case Color::Blue:
            return ST7735_BLUE;

        case Color::Yellow:
            return ST7735_YELLOW;
    }

    return ST7735_BLACK; // Fallback for an unexpected color value.
}

// Display the welcome screen with a border and centered title.
void DisplayManager::showTitle() {
    fillScreen(ST7735_BLACK); // Clear previous screen content.

    drawRectWH(2, 2, 124, 156, ST7735_CYAN); // Outer screen border.

    drawRectWH(12, 48, 104, 32, ST7735_CYAN); // Box around the title.
    drawText(37, 60, "THE CAGES", ST7735_CYAN, ST7735_BLACK, 1);

    drawText(31, 100, "Press ENTER", ST7735_WHITE, ST7735_BLACK, 1);
    drawText(31, 115, "to continue", ST7735_WHITE, ST7735_BLACK, 1);
}

// Display the menu and draw a box around the selected game.
void DisplayManager::showMenu(int selected) {
    fillScreen(ST7735_BLACK); // Clear the previous menu and selection box.

    drawRectWH(2, 2, 124, 156, ST7735_CYAN); // Outer screen border.
    drawText(34, 15, "CHALLENGES", ST7735_CYAN, ST7735_BLACK, 1);
    drawLine(12, 32, 115, 32, ST7735_CYAN); // Heading separator.

    // Highlight Accuracy when selected equals 0.
    if (selected == 0) {
        drawRectWH(12, 45, 104, 24, ST7735_YELLOW);
    }

    drawText(40, 53, "Accuracy", selected == 0 ? ST7735_YELLOW : ST7735_WHITE, ST7735_BLACK, 1);

    // Highlight Reaction when selected equals 1.
    if (selected == 1) {
        drawRectWH(12, 75, 104, 24, ST7735_YELLOW);
    }

    drawText(40, 83, "Reaction", selected == 1 ? ST7735_YELLOW : ST7735_WHITE, ST7735_BLACK, 1);

    // Highlight Memory when selected equals 2.
    if (selected == 2) {
        drawRectWH(12, 105, 104, 24, ST7735_YELLOW);
    }

    drawText( 46, 113, "Memory", selected == 2 ? ST7735_YELLOW : ST7735_WHITE, ST7735_BLACK, 1);

    // Display the selection instruction beneath the game choices.
    drawText(19, 140, "ENTER to select", ST7735_WHITE, ST7735_BLACK, 1);
}

// Display instructions for the selected game and an ENTER prompt.
void DisplayManager::showInstructions(int mode) {
    fillScreen(ST7735_BLACK); 

    drawRectWH(2, 2, 124, 156, ST7735_CYAN); // Outer screen border.
    drawLine(12, 32, 115, 32, ST7735_CYAN); // Heading separator.

    switch (mode) {
        case 0: // Accuracy instructions.
            drawText(40, 15, "ACCURACY", ST7735_CYAN, ST7735_BLACK, 1);
            drawText(10, 50, "Press the matching", ST7735_WHITE, ST7735_BLACK, 1);
            drawText(19, 65, "colored button.", ST7735_WHITE, ST7735_BLACK, 1);
            drawText(13, 90, "Get as many right", ST7735_WHITE, ST7735_BLACK, 1);
            drawText(13, 105, "before time ends!", ST7735_WHITE, ST7735_BLACK, 1);
            break;

        case 1: // Reaction instructions.
            drawText(40, 15, "REACTION", ST7735_CYAN, ST7735_BLACK, 1);
            drawText(10, 55, "Wait for a circle.", ST7735_WHITE, ST7735_BLACK, 1);
            drawText(10, 80, "Press its matching", ST7735_WHITE, ST7735_BLACK, 1);
            drawText(19, 95, "button quickly!", ST7735_WHITE, ST7735_BLACK, 1);
            break;

        case 2: // Memory instructions.
            drawText(46, 15, "MEMORY", ST7735_CYAN, ST7735_BLACK, 1);
            drawText(13, 45, "Watch the colors.", ST7735_WHITE, ST7735_BLACK, 1);
            drawText(7, 65, "Repeat the sequence", ST7735_WHITE, ST7735_BLACK, 1);
            drawText(10, 80, "using the buttons.", ST7735_WHITE, ST7735_BLACK, 1);
            drawText(19, 105, "Beat the timer!", ST7735_WHITE, ST7735_BLACK, 1);
            break;
    }

    // Display the start instruction inside a yellow outline.
    drawRectWH(12, 130, 104, 22, ST7735_YELLOW);
    drawText(22, 137, "ENTER to start", ST7735_YELLOW, ST7735_BLACK, 1);
}

void DisplayManager::showAccuracyPrompt(Color target, unsigned secondsLeft){
    fillScreen(ST7735_BLACK);

}

#include "display_manager.h"

void DisplayManager::init() {
    screen_.init();
}

void DisplayManager::clear(std::uint16_t color) {
    screen_.clear(color);
}

void DisplayManager::drawRectangle(std::uint8_t x, std::uint8_t y,
                                    std::uint8_t width, std::uint8_t height,
                                    std::uint16_t color) {
    screen_.rectangle(x, y, width, height, color);
}

void DisplayManager::drawText(std::uint8_t x, std::uint8_t y, const char* text,
                              std::uint16_t color, std::uint8_t scale,
                              std::uint16_t background) {
    screen_.text(x, y, text, color, scale, background);
}
