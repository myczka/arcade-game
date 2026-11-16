// --------------------------------------------------------------------------
// ST7735-library (hw-specific implementations)
//
// If you want to port the library to a different platform, change the
// implementation if necessary. Don't forget to change hw.h too.
//
// Author: Bernhard Bablok
//
// https://github.com/bablokb/pico-st7735
// --------------------------------------------------------------------------

#include "hw.h"
#include "hardware/dma.h"

// Reserved on first use and reused for this single display.
static int tx_dma = -1;
static int rx_dma = -1;

void tft_spi_init() {
  gpio_init(SPI_TFT_CS);
  gpio_set_dir(SPI_TFT_CS, GPIO_OUT);
  gpio_put(SPI_TFT_CS, 1);                        // Chip select is active-low

  gpio_init(SPI_TFT_DC);
  gpio_set_dir(SPI_TFT_DC, GPIO_OUT);
  gpio_put(SPI_TFT_DC, 0);                        // Chip select is active-low

  gpio_init(SPI_TFT_RST);
  gpio_set_dir(SPI_TFT_RST, GPIO_OUT);
  gpio_put(SPI_TFT_RST, 0);
}

void tft_write_pixels(const uint16_t *pixels, uint16_t width,
                      uint16_t height, uint16_t stride) {
  if(pixels == NULL || width == 0 || height == 0 || width > stride)
    return;

  if(tx_dma < 0) {
    tx_dma = dma_claim_unused_channel(false);
    if(tx_dma >= 0) {
      rx_dma = dma_claim_unused_channel(false);
      if(rx_dma < 0) {
        dma_channel_unclaim(tx_dma);
        tx_dma = -1;
      }
    }
  }

  // A 16-bit SPI word sends the RGB565 high byte first without a copy.
  spi_set_format(SPI_TFT_PORT, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  const uint32_t count = width == stride ? (uint32_t)width * height : width;
  const uint16_t transfers = width == stride ? 1 : height;

  if(tx_dma >= 0) {
    dma_channel_config tx_config = dma_channel_get_default_config(tx_dma);
    channel_config_set_transfer_data_size(&tx_config, DMA_SIZE_16);
    channel_config_set_read_increment(&tx_config, true);
    channel_config_set_write_increment(&tx_config, false);
    channel_config_set_dreq(&tx_config, spi_get_dreq(SPI_TFT_PORT, true));

    dma_channel_config rx_config = dma_channel_get_default_config(rx_dma);
    channel_config_set_transfer_data_size(&rx_config, DMA_SIZE_16);
    channel_config_set_read_increment(&rx_config, false);
    channel_config_set_write_increment(&rx_config, false);
    channel_config_set_dreq(&rx_config, spi_get_dreq(SPI_TFT_PORT, false));

    // SPI receives a word for every transmitted word, even without MISO wired.
    volatile uint16_t discard;
    for(uint16_t row = 0; row < transfers; row++) {
      dma_channel_configure(rx_dma, &rx_config, &discard,
                            &spi_get_hw(SPI_TFT_PORT)->dr, count, false);
      dma_channel_configure(tx_dma, &tx_config, &spi_get_hw(SPI_TFT_PORT)->dr,
                            pixels + row * stride, count, false);
      dma_start_channel_mask((1u << tx_dma) | (1u << rx_dma));
      dma_channel_wait_for_finish_blocking(tx_dma);
      dma_channel_wait_for_finish_blocking(rx_dma);
    }
  } else {
    // Keep the display usable if another component has claimed all channels.
    for(uint16_t row = 0; row < transfers; row++)
      spi_write16_blocking(SPI_TFT_PORT, pixels + row * stride, count);
  }

  // DMA completion alone does not mean the last SPI bit has left the pin.
  while(spi_is_busy(SPI_TFT_PORT))
    tight_loop_contents();
  spi_set_format(SPI_TFT_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}
