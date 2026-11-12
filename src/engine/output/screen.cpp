#include "screen.h"

#include "hardware/spi.h"
#include "pico/stdlib.h"

extern "C" {
#include "ST7735_TFT.h"
void tft_spi_init();
}

static uint16_t framebuffer[Screen::Height][Screen::Width];

void Screen::init() {
    spi_init(SPI_TFT_PORT, 10 * 1000 * 1000);
    spi_set_format(SPI_TFT_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);
    tft_spi_init(); // CS, DC, and reset are ordinary GPIO outputs.
    TFT_BlackTab_Initialize();
    setTextWrap(false);
    clear();
}

void Screen::clear(std::uint16_t color) {
    fillScreen(color);
}

void Screen::rectangle(std::uint8_t x, std::uint8_t y,
                       std::uint8_t width, std::uint8_t height,
                       std::uint16_t color) {
    if (x >= Width || y >= Height || width == 0 || height == 0) {
        return;
    }
    if (width > Width - x) width = Width - x;
    if (height > Height - y) height = Height - y;
    fillRectangle(x, y, width, height, color);
}

void Screen::text(std::uint8_t x, std::uint8_t y, const char* value,
                  std::uint16_t color, std::uint8_t scale,
                  std::uint16_t background) {
    if (value == nullptr || scale == 0 || x >= Width || y >= Height) return;
    const unsigned cellWidth = 6u * scale;
    const unsigned cellHeight = 7u * scale;
    if (cellHeight > Height - y) return;

    unsigned cursor = x;
    while (*value != '\0' && cursor + cellWidth <= Width) {
        // The driver draws only five glyph columns; clear the spacing too.
        rectangle(static_cast<std::uint8_t>(cursor), y,
                  static_cast<std::uint8_t>(cellWidth),
                  static_cast<std::uint8_t>(cellHeight), background);
        drawChar(static_cast<std::uint8_t>(cursor), y,
                 static_cast<std::uint8_t>(*value++), color, background, scale);
        cursor += cellWidth;
    }
}
