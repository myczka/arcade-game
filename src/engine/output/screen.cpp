#include "screen.h"
#include "hardware/spi.h" 
#include "pico/stdlib.h"

void Screen::init() {

    spi_init(SPI_PORT, 1000000);

    // Connect the clock and transmit GPIO pins to the SPI peripheral.
    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);

    TFT_BlackTab_Initialize();
    setTextWrap(true);

    // Start off with an empty black screen.
    fillScreen(ST7735_BLACK);
}