// TFT.h
#pragma once
#include <SPI.h>
#include "gpio.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace mpsmv2_tft {

#define NOP 0x00
#define SWRESET 0x01
#define SLEEP_OUT 0x11
#define COMMAND_PREFIX 0x20
#define GAMMA_SET 0x26
#define DISPLAY_OFF 0x28
#define DISPLAY_ON 0x29
#define COLUMN_ADDR 0x2A
#define PAGE_ADDR 0x2B
#define MEMORY_WRITE 0x2C
#define COL_MOD 0x3A
#define MADCTL 0x36
#define PRCH_SET 0xB2

#define EXT_CMD 0x80

#define ST7796_VENDOR 0xFF
#define VENDOR_PAGE0 0x00
#define VENDOR_PAGE1 0x01
#define VENDOR_PAGE2 0x02
#define VENDOR_PAGE80 0x80
#define VENDOR_PAGE81 0x81

#define PUSH_BUTTON 0    // Encoder pushbutton
#define ENCODER_LEFT 5   // FIXME: These maybe backwards, need to test
#define ENDCODE_RIGHT 4  // ^^
#define COMMAND_LATCH_PIN 15
#define PIXEL_LATCH_PIN 12

constexpr int GRAM_HEIGHT = 854;
constexpr int GRAM_WIDTH = 480;

// Convert 8-bit RGB to 16-bit RGB565
#define RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))
#define CMD_ON GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, CMD_MASK);   // CS LOW
#define CMD_OFF GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, CMD_MASK);  // CS HIGH
class MPSMV2_TFT;
using mpsmv2_tft_writer_t = display::DisplayWriter<MPSMV2_TFT>;

class MPSMV2_TFT : public display::DisplayBuffer {
 private:
  uint16_t _rotation = 90;
  uint16_t _width = GRAM_WIDTH;
  uint16_t _height = GRAM_HEIGHT;
  void write_data_rgb(uint16_t color, uint32_t repeats);
  void display_send_command(uint8_t param_1);
  void display_send_command(uint8_t command, uint8_t low_param, uint8_t high_param);
  void send_display_register(uint8_t param_1, uint8_t param_2, uint8_t param_3);
  void setMADCTL(uint8_t value);
  void latch_pixel_data();
  void latch_command();
  uint32_t setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  uint16_t last_color_written = 0xFFFF;

 public:
  MPSMV2_TFT() {}

  void set_writer(mpsmv2_tft_writer_t &&writer) { this->writer_local_ = writer; }

  void setup() override;
  void dump_config() override;
  void update() override;
  void begin();
  void displayOn();
  void displayOff();
  int get_height_internal() override { return this->_height; };
  int get_width_internal() override { return this->_width; };
  void draw_pixel_at(int x, int y, Color color) override;
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  void set_rotation(uint16 r);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillScreen(uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

 protected:
  mpsmv2_tft_writer_t writer_local_{};
};
}  // namespace mpsmv2_tft
}  // namespace esphome
