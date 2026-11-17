#include "display_manager.h"

#include "hardware/spi.h"
#include "pico/stdlib.h"

extern "C" {
#include "ST7735_TFT.h"
#include "hw.h"
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

void DisplayManager::clear(uint16_t color) {
    fillScreen(color);
}

void DisplayManager::present(int16_t x, int16_t y, uint8_t width, uint8_t height) {
    presentRectangle(x, y, width, height);
}

void DisplayManager::drawRectangle(int16_t x, int16_t y, uint8_t width, uint8_t height, uint16_t color) {
    fillRectangle(x, y, width, height, color);
}

void DisplayManager::drawSprite(int16_t x, int16_t y, const uint16_t* pixels, uint8_t width, uint8_t height) {
    drawBitmap(x, y, pixels, width, height, Transparent);
}

void DisplayManager::drawText(uint8_t x, uint8_t y, const char* text, uint16_t color, uint8_t scale, uint16_t background) {
    if (text == nullptr || scale == 0 || x >= Width || y >= Height) return;
    const unsigned cellWidth = 6u * scale;
    const unsigned cellHeight = 7u * scale;
    if (cellHeight > Height - y) return;

    unsigned cursor = x;
    uint16_t glyph_background = background;

    if (background == Transparent) {
        glyph_background = color;
    }

    while (*text != '\0' && cursor + cellWidth <= Width) {
        // The driver draws only five glyph columns; clear the spacing too.
    if (background != Transparent) {
        drawRectangle(static_cast<uint8_t>(cursor), y, static_cast<uint8_t>(cellWidth), static_cast<uint8_t>(cellHeight), background);
    }
    
    drawChar(static_cast<uint8_t>(cursor), y, static_cast<uint8_t>(*text++), color, glyph_background, scale);
cursor += cellWidth;
    }
}
