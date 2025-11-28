// TFT.cpp
#include "TFT.h"
#include "esphome/components/display/display.h"

namespace esphome {
namespace mpsmv2_tft {

static const char *const TAG = "MPSMV2_tft";
static const uint32_t LATCH_MASK = (1 << PIXEL_LATCH_PIN);
static const uint32_t CMD_MASK = (1 << COMMAND_LATCH_PIN);

void MPSMV2_TFT::setup() {
  SPI.begin();
  SPI.setFrequency(8000000);  // 8 mhz
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  pinMode(PIXEL_LATCH_PIN, OUTPUT);
  pinMode(COMMAND_LATCH_PIN, OUTPUT);
  // default state, command high, pixel data high:
  GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, CMD_MASK);
  GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, LATCH_MASK);
  delay(100);

  this->display_send_command(NOP);
  this->display_send_command(SWRESET);
  delay(100);

  this->display_send_command(COL_MOD, 0x00, 0x66);   // 16 bit RGB565
  this->display_send_command(MADCTL, 0x00, 0x00);    // portrait, no swap
  this->display_send_command(PRCH_SET, 0x82, 0x20);  // panel timing
  this->display_send_command(SLEEP_OUT);
  this->display_send_command(DISPLAY_OFF);
}

void MPSMV2_TFT::dump_config() {
  ESP_LOGCONFIG("mpsmv2_tft", "MPSMV2_TFT display");
  ESP_LOGCONFIG("mpsmv2_tft", "Resolution: %dx%d", GRAM_WIDTH, GRAM_HEIGHT);
}

void MPSMV2_TFT::update() {
  // Nothing to do here for now
}

void MPSMV2_TFT::display_send_command(uint8_t command) {
  CMD_ON;
  SPI.write(COMMAND_PREFIX);
  SPI.write(command);
  SPI.write(0);
  SPI.write(0);
  this->latch_command();
  CMD_OFF;
}

void MPSMV2_TFT::display_send_command(uint8_t command, uint8_t low_param, uint8_t high_param) {
  CMD_ON;
  SPI.write(COMMAND_PREFIX);
  SPI.write(command);
  SPI.write(0);
  SPI.write(low_param);
  this->latch_command();
  SPI.write(0x40);
  SPI.write(high_param);
  this->latch_command();
  CMD_OFF;
}

void MPSMV2_TFT::displayOff() {
  this->display_send_command(DISPLAY_OFF);  // display off
}

void MPSMV2_TFT::displayOn() {
  this->display_send_command(DISPLAY_ON);  // display on
}

void MPSMV2_TFT::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) { this->fillRect(x, y, 1, h, color); }

void MPSMV2_TFT::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) { this->fillRect(x, y, w, 1, color); }

void MPSMV2_TFT::draw_pixel_at(int x, int y, Color color) {
  if (x < 0 || y < 0 || x >= _width || y >= _height)
    return;

  const uint16_t pix_col = RGB565(color.r, color.g, color.b);
  this->setAddrWindow(x, y, x, y);
  this->write_data_rgb(pix_col, 1);
}

void MPSMV2_TFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  // Calculate bottom-right coordinates
  uint16_t x1 = x + w - 1;
  uint16_t y1 = y + h - 1;

  // Get total pixels from setAddrWindow (handles mapping)
  const uint32_t total_pixels = this->setAddrWindow(x, y, x1, y1);

  // Write the first pixel
  this->write_data_rgb(color, total_pixels);
}

void MPSMV2_TFT::fillScreen(uint16_t color) { this->fillRect(0, 0, this->_width, this->_height, color); }

IRAM_ATTR inline void MPSMV2_TFT::latch_command() {
  GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, CMD_MASK);  // HIGH
  GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, CMD_MASK);  // LOW
}

IRAM_ATTR inline void MPSMV2_TFT::latch_pixel_data() {
  GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, LATCH_MASK);  // LOW
  GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, LATCH_MASK);  // HIGH
}

void MPSMV2_TFT::send_display_register(uint8_t param_1, uint8_t param_2, uint8_t param_3) {
  uint8_t data;
  uint8_t data_00;
  uint8_t data_01;

  data = param_2 & 0xff;
  data_00 = param_1 & 0xff;
  data_01 = param_3 & 0xff;
  CMD_ON;
  SPI.write(0x20);
  SPI.write(data_00);
  SPI.write(0);
  SPI.write(data);
  SPI.write(0x40);
  SPI.write(data_01);
  CMD_OFF;
}

uint32_t MPSMV2_TFT::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  this->send_display_register(COLUMN_ADDR, 0, x0 >> 8);
  this->send_display_register(COLUMN_ADDR, 1, x0 & 0xff);
  this->send_display_register(COLUMN_ADDR, 2, x1 >> 8);
  this->send_display_register(COLUMN_ADDR, 3, x1 & 0xff);
  this->send_display_register(PAGE_ADDR, 0, y0 >> 8);
  this->send_display_register(PAGE_ADDR, 1, y0 & 0xff);
  this->send_display_register(PAGE_ADDR, 2, y1 >> 8);
  this->send_display_register(PAGE_ADDR, 3, y1 & 0xff);
  this->display_send_command(MEMORY_WRITE);
  uint32_t w = (x1 - x0) + 1;
  uint32_t h = (y1 - y0) + 1;

  return w * h;
}

void MPSMV2_TFT::setMADCTL(uint8_t value) {
  CMD_ON;

  // Write the MADCTL command
  SPI.write(0x20);  // command prefix
  SPI.write(0x36);  // MADCTL
  this->latch_command();

  // Write the MADCTL parameter
  SPI.write(0x40);   // parameter prefix
  SPI.write(value);  // bitfield we want to test
  this->latch_command();

  CMD_OFF;
}

void MPSMV2_TFT::set_rotation(uint8_t r) {
  uint8_t madctl = 0x00;  // default
  switch (r & 0x03) {     // m = 0..3
    case 0:               // portrait
      madctl = 0x00;
      this->_width = GRAM_WIDTH;
      this->_height = GRAM_HEIGHT;
      break;
    case 1:  // landscape -90°
      madctl = 0xA0;
      this->_width = GRAM_HEIGHT;
      this->_height = GRAM_WIDTH;
      break;
    case 2:  // portrait mirrored X
      madctl = 0x20;
      this->_width = GRAM_HEIGHT;
      this->_height = GRAM_WIDTH;
      break;
    case 3:  // portrait mirrored Y
      madctl = 0x40;
      this->_width = GRAM_WIDTH;
      this->_height = GRAM_HEIGHT;
      break;
  }

  this->setMADCTL(madctl);
  delay(50);
  this->display_send_command(DISPLAY_OFF);
  delay(50);
  this->display_send_command(DISPLAY_ON);
}

void MPSMV2_TFT::write_data_rgb(uint16_t color, uint32_t repeats) {
  for (uint32_t i = 0; i < repeats; i++) {
    // optimize for repeating colors
    if (repeats == 1 || color != this->last_color_written) {
      // send bytes in SR order
      SPI.write(color & 0xFF);
      SPI.write(color >> 8);
      this->last_color_written = color;
    }

    // strobe WR for the pixel
    this->latch_pixel_data();

    // occasionally yield for multitasking
    if ((i & 0x03FF) == 0)
      yield();
  }
}
}  // namespace mpsmv2_tft
}  // namespace esphome
