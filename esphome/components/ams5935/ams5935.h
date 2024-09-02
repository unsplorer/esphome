#pragma once

#include <cstdint>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "Wire.h"
#include "Arduino.h"

namespace esphome {
namespace ams5935 {
enum Transducer {
  AMS5935_0002_D,
  AMS5935_0005_D,
  AMS5935_0010_D,
  AMS5935_0020_D,
  AMS5935_0035_D,
  AMS5935_0050_D,
  AMS5935_0100_D,
  AMS5935_0200_D,
  AMS5935_0350_D,
  AMS5935_0500_D,
  AMS5935_1000_D,
  AMS5935_0002_D_N,
  AMS5935_0005_D_N,
  AMS5935_0010_D_N,
  AMS5935_0020_D_N,
  AMS5935_0050_D_N,
  AMS5935_0100_D_N,
  AMS5935_0200_D_N,
  AMS5935_0350_D_N,
  AMS5935_1000_D_N,
  AMS5935_0001_D_B,
  AMS5935_0002_D_B,
  AMS5935_0005_D_B,
  AMS5935_0010_D_B,
  AMS5935_0020_D_B,
  AMS5935_0035_D_B,
  AMS5935_0050_D_B,
  AMS5935_0100_D_B,
  AMS5935_0200_D_B,
  AMS5935_0350_D_B,
  AMS5935_0500_D_B,
  AMS5935_1000_D_B,
  AMS5935_0001_D_B_N,
  AMS5935_0002_D_B_N,
  AMS5935_0005_D_B_N,
  AMS5935_0010_D_B_N,
  AMS5935_0020_D_B_N,
  AMS5935_0050_D_B_N,
  AMS5935_0100_D_B_N,
  AMS5935_0200_D_B_N,
  AMS5935_0350_D_B_N,
  AMS5935_1000_D_B_N,
  AMS5935_0500_A,
  AMS5935_1000_A,
  AMS5935_1500_A,
  AMS5935_2000_A,
  AMS5935_1200_B
};
class Ams5935 : public PollingComponent, public sensor::Sensor, public i2c::I2CDevice {
 public:
  Ams5935() : PollingComponent(5000) {}
  void dump_config() override;
  void setup() override;
  void update() override;
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_pressure_sensor(sensor::Sensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }
  void set_transducer_type(Transducer model);

  // transducer
 protected:
  int begin_();
  int read_sensor_();
  float get_pressure_pa_();
  float get_temperature_c_();

  Transducer type_;
  // struct to hold sensor data
  struct Data {
    float pressure_pa_;
    float temperature_c_;
  };
  Data data_;
  // buffer for I2C data
  uint8_t buffer_[7];

  // read commands:
  //    To start the measurement cycle, the master has to send a data request: 0xAA for single measurement or
  //    0xAD for four-fold oversampling.
  const uint8_t single_measurment_command_ = 0xAA;
  const uint8_t four_fold_oversampling_command_ = 0xAD;
  // read request wait times:
  const uint8_t single_measurment_processing_time_ = 4;       // 4ms
  const uint8_t four_fold_oversampling_processing_time = 15;  // 15ms
  // maximum number of attempts to talk to sensor
  const size_t max_attempts_ = 10;
  // track success of reading from sensor
  int status_;
  // pressure digital output, counts
  uint32_t pressure_counts_;
  // temperature digital output, counts
  uint32_t temperature_counts_;
  // min and max pressure, millibar
  int p_min_;
  int p_max_;
  // conversion millibar to PA
  const float mbar_to_pa_ = 100.0f;
  // digital output at maximum pressure
  const int dig_out_p_max_ = 1 << 24;
  // digital output at minimum pressure
  const float dig_out_p_min_ = dig_out_p_max_ * 0.1;
  // min and max pressures, millibar
  const int ams5935_0002_d_p_min_ = 0;
  const int ams5935_0002_d_p_max_ = 2;
  const int ams5935_0005_d_p_min_ = 0;
  const int ams5935_0005_d_p_max_ = 5;
  const int ams5935_0010_d_p_min_ = 0;
  const int ams5935_0010_d_p_max_ = 10;
  const int ams5935_0020_d_p_min_ = 0;
  const int ams5935_0020_d_p_max_ = 20;
  const int ams5935_0035_d_p_min_ = 0;
  const int ams5935_0035_d_p_max_ = 35;
  const int ams5935_0050_d_p_min_ = 0;
  const int ams5935_0050_d_p_max_ = 50;
  const int ams5935_0100_d_p_min_ = 0;
  const int ams5935_0100_d_p_max_ = 100;
  const int ams5935_0200_d_p_min_ = 0;
  const int ams5935_0200_d_p_max_ = 200;
  const int ams5935_0350_d_p_min_ = 0;
  const int ams5935_0350_d_p_max_ = 350;
  const int ams5935_0500_d_p_min_ = 0;
  const int ams5935_0500_d_p_max_ = 500;
  const int ams5935_1000_d_p_min_ = 0;
  const int ams5935_1000_d_p_max_ = 1000;
  const int ams5935_0002_d_n_p_min_ = 0;
  const int ams5935_0005_d_n_p_min_ = 0;
  const int ams5935_0010_d_n_p_min_ = 0;
  const int ams5935_0020_d_n_p_min_ = 0;
  const int ams5935_0050_d_n_p_min_ = 0;
  const int ams5935_0100_d_n_p_min_ = 0;
  const int ams5935_0200_d_n_p_min_ = 0;
  const int ams5935_0350_d_n_p_min_ = 0;
  const int ams5935_1000_d_n_p_min_ = 0;
  const int ams5935_0001_d_b_p_min_ = -1;
  const int ams5935_0002_d_b_p_min_ = -2;
  const int ams5935_0005_d_b_p_min_ = -5;
  const int ams5935_0010_d_b_p_min_ = -10;
  const int ams5935_0020_d_b_p_min_ = -20;
  const int ams5935_0035_d_b_p_min_ = -35;
  const int ams5935_0050_d_b_p_min_ = -50;
  const int ams5935_0100_d_b_p_min_ = -100;
  const int ams5935_0200_d_b_p_min_ = -200;
  const int ams5935_0350_d_b_p_min_ = -350;
  const int ams5935_0500_d_b_p_min_ = -500;
  const int ams5935_1000_d_b_p_min_ = -1000;
  const int ams5935_0002_d_n_p_max_ = 2;
  const int ams5935_0005_d_n_p_max_ = 5;
  const int ams5935_0010_d_n_p_max_ = 10;
  const int ams5935_0020_d_n_p_max_ = 20;
  const int ams5935_0050_d_n_p_max_ = 50;
  const int ams5935_0100_d_n_p_max_ = 100;
  const int ams5935_0200_d_n_p_max_ = 200;
  const int ams5935_0350_d_n_p_max_ = 350;
  const int ams5935_1000_d_n_p_max_ = 1000;
  const int ams5935_0001_d_b_p_max_ = 1;
  const int ams5935_0002_d_b_p_max_ = 2;
  const int ams5935_0005_d_b_p_max_ = 5;
  const int ams5935_0010_d_b_p_max_ = 10;
  const int ams5935_0020_d_b_p_max_ = 20;
  const int ams5935_0035_d_b_p_max_ = 35;
  const int ams5935_0050_d_b_p_max_ = 50;
  const int ams5935_0100_d_b_p_max_ = 100;
  const int ams5935_0200_d_b_p_max_ = 200;
  const int ams5935_0350_d_b_p_max_ = 350;
  const int ams5935_0500_d_b_p_max_ = 500;
  const int ams5935_1000_d_b_p_max_ = 1000;
  const int ams5935_0001_d_b_n_p_min_ = -1;
  const int ams5935_0002_d_b_n_p_min_ = -2;
  const int ams5935_0005_d_b_n_p_min_ = -5;
  const int ams5935_0010_d_b_n_p_min_ = -10;
  const int ams5935_0020_d_b_n_p_min_ = -20;
  const int ams5935_0050_d_b_n_p_min_ = -50;
  const int ams5935_0100_d_b_n_p_min_ = -100;
  const int ams5935_0200_d_b_n_p_min_ = -200;
  const int ams5935_0350_d_b_n_p_min_ = -350;
  const int ams5935_1000_d_b_n_p_min_ = -1000;
  const int ams5935_0001_d_b_n_p_max_ = 1;
  const int ams5935_0002_d_b_n_p_max_ = 2;
  const int ams5935_0005_d_b_n_p_max_ = 5;
  const int ams5935_0010_d_b_n_p_max_ = 10;
  const int ams5935_0020_d_b_n_p_max_ = 20;
  const int ams5935_0050_d_b_n_p_max_ = 50;
  const int ams5935_0100_d_b_n_p_max_ = 100;
  const int ams5935_0200_d_b_n_p_max_ = 200;
  const int ams5935_0350_d_b_n_p_max_ = 350;
  const int ams5935_1000_d_b_n_p_max_ = 1000;
  const int ams5935_0500_a_p_min_ = 0;
  const int ams5935_1000_a_p_min_ = 0;
  const int ams5935_1500_a_p_min_ = 0;
  const int ams5935_2000_a_p_min_ = 0;
  const int ams5935_0500_a_p_max_ = 500;
  const int ams5935_1000_a_p_max_ = 1000;
  const int ams5935_1500_a_p_max_ = 1500;
  const int ams5935_2000_a_p_max_ = 2000;
  const int ams5935_1200_b_p_min_ = 700;
  const int ams5935_1200_b_p_max_ = 1200;
  void get_transducer_();
  int read_bytes_(uint32_t *pressure_counts, uint32_t *temperature_counts);

  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *pressure_sensor_{nullptr};
};

}  // namespace ams5935
}  // namespace esphome
