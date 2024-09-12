#include "ams5935.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ams5935 {
static const char *const TAG = "ams5935";

void Ams5935::set_transducer_type(Transducer model) { type_ = model; }

/* starts the I2C communication and sets the pressure and temperature ranges using getTransducer */
int Ams5935::begin_() {
  // setting the min and max pressure based on the chip
  this->get_transducer_();
  // checking to see if we can talk with the sensor
  for (size_t i = 0; i < max_attempts_; i++) {
    status_ = read_bytes_(&pressure_counts_, &temperature_counts_);
    if (status_ > 0) {
      break;
    }
    delay(10);
  }
  return status_;
}

/* reads data from the sensor */
int Ams5935::read_sensor_() {
  // get pressure and temperature counts off transducer
  this->status_ = read_bytes_(&this->pressure_counts_, &this->temperature_counts_);
  // convert counts to pressure, PA
  this->data_.pressure_pa_ =
      (((float) (this->pressure_counts_ - this->dig_out_p_min_)) /
           (((float) (this->dig_out_p_max_ - this->dig_out_p_min_)) / ((float) (this->p_max_ - this->p_min_))) +
       (float) this->p_min_);
  // convert counts to temperature, C
  this->data_.temperature_c_ = (float) (((this->temperature_counts_ * 165) / (float) dig_out_p_max_)) - 40.0f;
  return this->status_;
}

/* returns the pressure value, PA */
float Ams5935::get_pressure_pa_() { return this->data_.pressure_pa_; }

/* returns the temperature value, C */
float Ams5935::get_temperature_c_() { return this->data_.temperature_c_; }

/* sets the pressure range based on the chip */
void Ams5935::get_transducer_() {
  // setting the min and max pressures based on which transducer it is
  switch (this->type_) {
    case AMS5935_0002_D:
      this->p_min_ = this->ams5935_0002_d_p_min_;
      this->p_max_ = this->ams5935_0002_d_p_max_;
      break;
    case AMS5935_0005_D:
      this->p_min_ = this->ams5935_0005_d_p_min_;
      this->p_max_ = this->ams5935_0005_d_p_min_;
      break;
    case AMS5935_0010_D:
      this->p_min_ = this->ams5935_0010_d_p_min_;
      this->p_max_ = this->ams5935_0010_d_p_max_;
      break;
    case AMS5935_0020_D:
      this->p_min_ = this->ams5935_0020_d_p_min_;
      this->p_max_ = this->ams5935_0020_d_p_max_;
      break;
    case AMS5935_0035_D:
      this->p_min_ = this->ams5935_0035_d_p_min_;
      this->p_max_ = this->ams5935_0035_d_p_max_;
      break;
    case AMS5935_0050_D:
      this->p_min_ = this->ams5935_0050_d_p_min_;
      this->p_max_ = this->ams5935_0050_d_p_max_;
      break;
    case AMS5935_0100_D:
      this->p_min_ = this->ams5935_0100_d_p_min_;
      this->p_max_ = this->ams5935_0100_d_p_max_;
      break;
    case AMS5935_0200_D:
      this->p_min_ = this->ams5935_0200_d_p_min_;
      this->p_max_ = this->ams5935_0200_d_p_max_;
      break;
    case AMS5935_0350_D:
      this->p_min_ = this->ams5935_0350_d_p_min_;
      this->p_max_ = this->ams5935_0350_d_p_max_;
      break;
    case AMS5935_0500_D:
      this->p_min_ = this->ams5935_0500_d_p_min_;
      this->p_max_ = this->ams5935_0500_d_p_max_;
      break;
    case AMS5935_1000_D:
      this->p_min_ = this->ams5935_1000_d_p_min_;
      this->p_max_ = this->ams5935_1000_d_p_max_;
      break;
    case AMS5935_0002_D_N:
      this->p_min_ = this->ams5935_0002_d_n_p_min_;
      this->p_max_ = this->ams5935_0002_d_n_p_max_;
      break;
    case AMS5935_0005_D_N:
      this->p_min_ = this->ams5935_0005_d_n_p_min_;
      this->p_max_ = this->ams5935_0005_d_n_p_max_;
      break;
    case AMS5935_0010_D_N:
      this->p_min_ = this->ams5935_0010_d_n_p_min_;
      this->p_max_ = this->ams5935_0010_d_n_p_max_;
      break;
    case AMS5935_0020_D_N:
      this->p_min_ = this->ams5935_0020_d_n_p_min_;
      this->p_max_ = this->ams5935_0020_d_n_p_max_;
      break;
    case AMS5935_0050_D_N:
      this->p_min_ = this->ams5935_0050_d_n_p_min_;
      this->p_max_ = this->ams5935_0050_d_n_p_max_;
      break;
    case AMS5935_0100_D_N:
      this->p_min_ = this->ams5935_0100_d_n_p_min_;
      this->p_max_ = this->ams5935_0100_d_n_p_max_;
      break;
    case AMS5935_0200_D_N:
      this->p_min_ = this->ams5935_0200_d_n_p_min_;
      this->p_max_ = this->ams5935_0200_d_n_p_max_;
      break;
    case AMS5935_0350_D_N:
      this->p_min_ = this->ams5935_0350_d_n_p_min_;
      this->p_max_ = this->ams5935_0350_d_n_p_max_;
      break;
    case AMS5935_1000_D_N:
      this->p_min_ = this->ams5935_1000_d_n_p_min_;
      this->p_max_ = this->ams5935_1000_d_n_p_max_;
      break;
    case AMS5935_0001_D_B:
      this->p_min_ = this->ams5935_0001_d_b_p_min_;
      this->p_max_ = this->ams5935_0001_d_b_p_max_;
      break;
    case AMS5935_0002_D_B:
      this->p_min_ = this->ams5935_0002_d_b_p_min_;
      this->p_max_ = this->ams5935_0002_d_b_p_max_;
      break;
    case AMS5935_0005_D_B:
      this->p_min_ = this->ams5935_0005_d_b_p_min_;
      this->p_max_ = this->ams5935_0005_d_b_p_max_;
      break;
    case AMS5935_0010_D_B:
      this->p_min_ = this->ams5935_0010_d_b_p_min_;
      this->p_max_ = this->ams5935_0010_d_b_p_max_;
      break;
    case AMS5935_0020_D_B:
      this->p_min_ = this->ams5935_0020_d_b_p_min_;
      this->p_max_ = this->ams5935_0020_d_b_p_max_;
      break;
    case AMS5935_0035_D_B:
      this->p_min_ = this->ams5935_0035_d_b_p_min_;
      this->p_max_ = this->ams5935_0035_d_b_p_max_;
      break;
    case AMS5935_0050_D_B:
      this->p_min_ = this->ams5935_0050_d_b_p_min_;
      this->p_max_ = this->ams5935_0050_d_b_p_max_;
      break;
    case AMS5935_0100_D_B:
      this->p_min_ = this->ams5935_0100_d_b_p_min_;
      this->p_max_ = this->ams5935_0100_d_b_p_max_;
      break;
    case AMS5935_0200_D_B:
      this->p_min_ = this->ams5935_0200_d_b_p_min_;
      this->p_max_ = this->ams5935_0200_d_b_p_max_;
      break;
    case AMS5935_0350_D_B:
      this->p_min_ = this->ams5935_0350_d_b_p_min_;
      this->p_max_ = this->ams5935_0350_d_b_p_max_;
      break;
    case AMS5935_0500_D_B:
      this->p_min_ = this->ams5935_0500_d_b_p_min_;
      this->p_max_ = this->ams5935_0500_d_b_p_max_;
      break;
    case AMS5935_1000_D_B:
      this->p_min_ = this->ams5935_1000_d_b_p_min_;
      this->p_max_ = this->ams5935_1000_d_b_p_max_;
      break;
    case AMS5935_0001_D_B_N:
      this->p_min_ = this->ams5935_0001_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0001_d_b_n_p_max_;
      break;
    case AMS5935_0002_D_B_N:
      this->p_min_ = this->ams5935_0002_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0002_d_b_n_p_max_;
      break;
    case AMS5935_0005_D_B_N:
      this->p_min_ = this->ams5935_0005_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0005_d_b_n_p_max_;
      break;
    case AMS5935_0010_D_B_N:
      this->p_min_ = this->ams5935_0010_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0010_d_b_n_p_max_;
      break;
    case AMS5935_0020_D_B_N:
      this->p_min_ = this->ams5935_0020_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0020_d_b_n_p_max_;
      break;
    case AMS5935_0050_D_B_N:
      this->p_min_ = this->ams5935_0050_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0050_d_b_n_p_max_;
      break;
    case AMS5935_0100_D_B_N:
      this->p_min_ = this->ams5935_0100_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0100_d_b_n_p_max_;
      break;
    case AMS5935_0200_D_B_N:
      this->p_min_ = this->ams5935_0200_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0200_d_b_n_p_max_;
      break;
    case AMS5935_0350_D_B_N:
      this->p_min_ = this->ams5935_0350_d_b_n_p_min_;
      this->p_max_ = this->ams5935_0350_d_b_n_p_max_;
      break;
    case AMS5935_1000_D_B_N:
      this->p_min_ = this->ams5935_1000_d_b_n_p_min_;
      this->p_max_ = this->ams5935_1000_d_b_n_p_max_;
      break;
    case AMS5935_0500_A:
      this->p_min_ = this->ams5935_0500_a_p_min_;
      this->p_max_ = this->ams5935_0500_a_p_max_;
      break;
    case AMS5935_1000_A:
      this->p_min_ = this->ams5935_1000_a_p_min_;
      this->p_max_ = this->ams5935_1000_a_p_max_;
      break;
    case AMS5935_1500_A:
      this->p_min_ = this->ams5935_1500_a_p_min_;
      this->p_max_ = this->ams5935_1500_a_p_max_;
      break;
    case AMS5935_2000_A:
      this->p_min_ = this->ams5935_2000_a_p_min_;
      this->p_max_ = this->ams5935_2000_a_p_max_;
      break;
    case AMS5935_1200_B:
      this->p_min_ = this->ams5935_1200_b_p_min_;
      this->p_max_ = this->ams5935_1200_b_p_max_;
      break;
  }
}

/* reads pressure and temperature and returns values in counts */
int Ams5935::read_bytes_(uint32_t *pressure_counts, uint32_t *temperature_counts) {
  static uint64_t last_read_time = 0;
  static bool read_requested = false;
  const uint64_t now = millis();

  if (!read_requested) {
    uint8_t read_request_data[1];
    read_request_data[0] = this->single_measurment_command_;
    i2c::ErrorCode write_err = this->write(read_request_data, 1, true);
    read_requested = true;
  }

  // wait for the ams5935 to process // non-blocking
  if (read_requested && (now - last_read_time > this->single_measurment_processing_time_)) {
    i2c::ErrorCode err = this->read(this->buffer_, sizeof(this->buffer_));
    if (err != i2c::ERROR_OK) {
      this->status_ = -1;
    } else {
      *pressure_counts =
          ((uint32_t) this->buffer_[1] << 16) | ((uint32_t) this->buffer_[2] << 8) | ((uint32_t) this->buffer_[3]);
      ESP_LOGD(TAG, "Raw Pressure Data: %X", pressure_counts);
      *temperature_counts =
          ((uint32_t) this->buffer_[4] << 16) | ((uint32_t) this->buffer_[5] << 8) | ((uint32_t) this->buffer_[6]);
      ESP_LOGD(TAG, "Raw Temperature Data: %X", temperature_counts);

      this->status_ = 1;
    }
    read_requested = false;
    last_read_time = now;
  }

  return this->status_;
}

void Ams5935::setup() {
  if (this->begin_() < 0) {
    ESP_LOGE(TAG, "Failed to read from Ams5935");
    this->mark_failed();
  }
}

void Ams5935::update() {
  this->read_sensor_();
  float temperature = this->get_temperature_c_();
  float pressure = this->get_pressure_pa_();

  ESP_LOGD(TAG, "Got pressure=%.3fmBar %.3fpa temperature=%.1f°C", pressure, pressure * this->mbar_to_pa_, temperature);
  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(temperature);
  if (this->pressure_sensor_ != nullptr)
    this->pressure_sensor_->publish_state(pressure * this->mbar_to_pa_);
}

void Ams5935::dump_config() {
  ESP_LOGCONFIG(TAG, "Ams5935:");
  LOG_I2C_DEVICE(this);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("  ", "Pressure", this->pressure_sensor_);
}

}  // namespace ams5935
}  // namespace esphome
