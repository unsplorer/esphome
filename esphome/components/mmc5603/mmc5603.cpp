#include "mmc5603.h"
#include "esphome/core/log.h"
#include "esphome.h"

namespace esphome {
namespace mmc5603 {

static const char *const TAG = "mmc5603";
static const uint8_t MMC5603_ADDRESS = 0x30;
static const uint8_t MMC56X3_PRODUCT_ID = 0x39;

static const uint8_t MMC56X3_DEFAULT_ADDRESS = 0x30;
static const uint8_t MMC56X3_CHIP_ID = 0x10;

static const uint8_t MMC56X3_ADDR_XOUT0 = 0x00;
static const uint8_t MMC56X3_ADDR_XOUT1 = 0x01;
static const uint8_t MMC56X3_ADDR_XOUT2 = 0x06;

static const uint8_t MMC56X3_ADDR_YOUT0 = 0x02;
static const uint8_t MMC56X3_ADDR_YOUT1 = 0x03;
static const uint8_t MMC56X3_ADDR_YOUT2 = 0x07;

static const uint8_t MMC56X3_ADDR_ZOUT0 = 0x04;
static const uint8_t MMC56X3_ADDR_ZOUT1 = 0x05;
static const uint8_t MMC56X3_ADDR_ZOUT2 = 0x08;

static const uint8_t MMC56X3_OUT_TEMP = 0x09;
static const uint8_t MMC56X3_STATUS_REG = 0x18;
static const uint8_t MMC56X3_CTRL0_REG = 0x1B;
static const uint8_t MMC56X3_CTRL1_REG = 0x1C;
static const uint8_t MMC56X3_CTRL2_REG = 0x1D;
static const uint8_t MMC5603_ODR_REG = 0x1A;

static const uint16_t measurement_processing_time = 10; // 10 milliseconds


void MMC5603Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MMC5603...");
  uint8_t id = 0;
  if (!this->read_byte(MMC56X3_PRODUCT_ID, &id)) {
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
  ESP_LOGE(TAG, "read Chip id %d",id);
  if (id != MMC56X3_CHIP_ID) {
    ESP_LOGCONFIG(TAG, "Chip Wrong");
    this->error_code_ = ID_REGISTERS;
    this->mark_failed();
    return;
  }

  // Software Reset. Writing “1”will cause the part to reset, similar to power-up. It will clear all registers 
  // and also re-read OTP as part of its startup routine. The power on time is 20mS.   
  if (!this->write_byte(MMC56X3_CTRL1_REG, 0x80)) {  // turn on set bit
    ESP_LOGCONFIG(TAG, "Control 1 Failed for set bit");
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
  // wait for device to init after reset
  delay(20);

  // Writing a 1 into this location will cause the chip to do the Set operation, which will allow large set 
  // current to flow through the sensor coils for 375ns. This bit is self-cleared at the end of Set 
  // operation.
  if (!this->write_byte(MMC56X3_CTRL0_REG, 0x08)) {  // turn on set bit
    ESP_LOGCONFIG(TAG, "Control 0 Failed for set bit");
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }  
  // wait some more
  delayMicroseconds(375);

  // Writing a 1 into this location will cause the chip to do the Reset operation, which will allow large 
  // reset current to flow through the sensor coils for 375ns. This bit is self-cleared at the end of Reset 
  // operation. 
  if (!this->write_byte(MMC56X3_CTRL0_REG, 0x10)) {
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
  delayMicroseconds(375);

  uint8_t ctrl_2 = 0;

  ctrl_2 &= ~0x10;  // turn off cmm_en bit
  if (!this->write_byte(MMC56X3_CTRL2_REG, ctrl_2)) {
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
  // make sure to set cmm rate to 0 - to completely disable it
  if (!this->write_byte(MMC5603_ODR_REG, 0x00)) {
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
}
void MMC5603Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MMC5603:");
  LOG_I2C_DEVICE(this);
  if (this->error_code_ == COMMUNICATION_FAILED) {
    ESP_LOGE(TAG, "Communication with MMC5603 failed!");
  } else if (this->error_code_ == ID_REGISTERS) {
    ESP_LOGE(TAG, "The ID registers don't match - Is this really an MMC5603?");
  }
  LOG_UPDATE_INTERVAL(this);

  LOG_SENSOR("  ", "X Axis", this->x_sensor_);
  LOG_SENSOR("  ", "Y Axis", this->y_sensor_);
  LOG_SENSOR("  ", "Z Axis", this->z_sensor_);
  LOG_SENSOR("  ", "Heading", this->heading_sensor_);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
}

float MMC5603Component::get_setup_priority() const { return setup_priority::DATA; }

void MMC5603Component::update() {
  static unsigned long last_measurement_time = 0;
  const unsigned long now = millis();
  static bool magnetic_measurement_requested = false;
  static bool temperature_measurement_requested = false;

  // take magnetic field measurement
  if (magnetic_measurement_requested == false){
    if (!this->write_byte(MMC56X3_CTRL0_REG, 0x01)) {
    this->status_set_warning();
      return;
    }
    ESP_LOGV(TAG, "Requesting magnetic reading.");
    magnetic_measurement_requested = true;
    last_measurement_time = now;
    delay(10);
  }

    // take temperature field measurement
  if (temperature_measurement_requested == false){
    if (!this->write_byte(MMC56X3_CTRL0_REG, 0x02)) {
    this->status_set_warning();
      return;
    }
    ESP_LOGV(TAG, "Requesting temperature reading.");
    temperature_measurement_requested = true;
    last_measurement_time = now;
  }

  if (((now - last_measurement_time) >= measurement_processing_time) && (magnetic_measurement_requested || temperature_measurement_requested)){

    uint8_t status = 0;
    ESP_LOGV(TAG, "Checking if data is ready.");
    if (!this->read_byte(MMC56X3_STATUS_REG, &status)) {
      this->status_set_warning();
      return;
    }
    ESP_LOGV(TAG, "Status %d.",status);
    bool temperature_measurement_ready = (status & 0x80) != 0;
    bool magnetic_measurement_ready =  (status & 0x40) != 0;


    if (temperature_measurement_ready){
      temperature_measurement_requested = false;

      // temperature data
      ESP_LOGV(TAG, "Temperature data is ready.");
      uint8_t raw_t = 0;
      if (!this->read_byte(MMC56X3_OUT_TEMP, &raw_t)) {
        ESP_LOGE(TAG,"Failed to read temperature");
        this->status_set_warning();
        // return;
      }
      const float temperature = -75 + (raw_t * 0.8);
      if (this->temperature_sensor_ != nullptr){
        this->temperature_sensor_->publish_state(temperature);
      }
    } 

    if (magnetic_measurement_ready){
      magnetic_measurement_requested = false;
      uint8_t buffer[9] = {0};
      if (!this->read_byte(MMC56X3_ADDR_XOUT0, &buffer[0]) || !this->read_byte(MMC56X3_ADDR_XOUT1, &buffer[1]) ||
          !this->read_byte(MMC56X3_ADDR_XOUT2, &buffer[2])) {
        this->status_set_warning();
        // return;
      }

      if (!this->read_byte(MMC56X3_ADDR_YOUT0, &buffer[3]) || !this->read_byte(MMC56X3_ADDR_YOUT1, &buffer[4]) ||
          !this->read_byte(MMC56X3_ADDR_YOUT2, &buffer[5])) {
        this->status_set_warning();
        // return;
      }

      if (!this->read_byte(MMC56X3_ADDR_ZOUT0, &buffer[6]) || !this->read_byte(MMC56X3_ADDR_ZOUT1, &buffer[7]) ||
          !this->read_byte(MMC56X3_ADDR_ZOUT2, &buffer[8])) {
        this->status_set_warning();
        // return;
      }

      int32_t raw_x = 0;
      raw_x |= buffer[0] << 12;
      raw_x |= buffer[1] << 4;
      raw_x |= buffer[2] << 0;

      const float x = 0.0625 * (raw_x - 524288);

      int32_t raw_y = 0;
      raw_y |= buffer[3] << 12;
      raw_y |= buffer[4] << 4;
      raw_y |= buffer[5] << 0;

      const float y = 0.0625 * (raw_y - 524288);

      int32_t raw_z = 0;
      raw_z |= buffer[6] << 12;
      raw_z |= buffer[7] << 4;
      raw_z |= buffer[8] << 0;

      const float z = 0.0625 * (raw_z - 524288);

      const float heading = atan2f(0.0f - x, y) * 180.0f / M_PI;

    
      if (this->x_sensor_ != nullptr)
        this->x_sensor_->publish_state(x);
      if (this->y_sensor_ != nullptr)
        this->y_sensor_->publish_state(y);
      if (this->z_sensor_ != nullptr)
        this->z_sensor_->publish_state(z);
      if (this->heading_sensor_ != nullptr)
        this->heading_sensor_->publish_state(heading);
    }
  }
}

}  // namespace mmc5603
}  // namespace esphome
