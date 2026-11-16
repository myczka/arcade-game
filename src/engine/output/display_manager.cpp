#include "display_manager.h"

#include "hardware/spi.h"
#include "pico/stdlib.h"

extern "C" {
#include "ST7735_TFT.h"
#include "hw.h"
}

// Display the welcome screen with a border and centered title.
void DisplayManager::showTitle() {
    clear(); // Clear previous screen content.

    drawRectWH(2, 2, 124, 156, ST7735_CYAN); // Outer screen border.

    drawRectWH(12, 48, 104, 32, ST7735_CYAN); // Box around the title.
    drawText(37, 60, "SWIMMY FISH", ST7735_CYAN, 1, Black);

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

    // Highlight Practice when selected equals 0.
    if (selected == 0) {
        drawRectWH(12, 45, 104, 24, ST7735_YELLOW);
    }

    drawText(40, 53, "PRACTICE", selected == 0 ? ST7735_YELLOW : ST7735_WHITE, 1, Black);

    // Highlight Swimmy Fish when selected equals 1.
    if (selected == 1) {
        drawRectWH(12, 75, 104, 24, ST7735_YELLOW);
    }

    drawText(40, 83, "SWIMMY FISH", selected == 1 ? ST7735_YELLOW : ST7735_WHITE, 1, Black);

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
        case 0: // Practice instructions.
            drawText(40, 15, "PRACTICE", ST7735_CYAN, 1, Black);
            drawText(10, 50, "Press the matching", ST7735_WHITE, 1, Black);
            drawText(19, 65, "colored button", ST7735_WHITE, 1, Black);
            drawText(16, 90, "when notes reach", ST7735_WHITE, 1, Black);
            drawText(19, 105, "the white zone.", ST7735_WHITE, 1, Black);
            break;

        case 1: // Swimmy Fish instructions.
            drawText(31, 15, "SWIMMY FISH", ST7735_CYAN, 1, Black);
            drawText(10, 55, "Swim through gaps.", ST7735_WHITE, 1, Black);
            drawText(16, 80, "Avoid the pipes.", ST7735_WHITE, 1, Black);
            drawText(7, 105, "Pass pipes to score", ST7735_WHITE, 1, Black);
            break;
    }

    // Display the start instruction inside a yellow outline.
    drawRectWH(12, 130, 104, 22, ST7735_YELLOW);
    drawText(22, 137, "ENTER to start", ST7735_YELLOW, 1, Black);
    present();
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
                                std::uint8_t width, std::uint8_t height) {
    drawBitmap(x, y, pixels, width, height, Transparent);
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
