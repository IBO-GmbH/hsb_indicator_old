#ifndef SENSOR_VALUE_READER_HPP
#define SENSOR_VALUE_READER_HPP

#pragma once

#include <Arduino.h>
#include <Wire.h>

#include <array>

#define SDA_Pin 0
#define SCL_Pin 26

class sensor_value_reader {
 public:
  sensor_value_reader(const int i2c_address_);
  ~sensor_value_reader() = default;

  bool sensor_value_init();
  double handle_read(const double offset);

 private:
  bool do_soft_reset();
  bool get_id_register();
  bool send_command(const std::uint16_t command);
  bool send_register_pointer(const std::uint8_t register_pointer);
  bool send_i2c(const std::uint8_t *to_send, const std::size_t size);
  bool read_i2c(std::uint8_t* to_read, const std::size_t size);

 private:
  int i2c_address;

  static constexpr std::uint8_t id_register = 0x0f;
  static constexpr std::uint8_t config_register = 0x01;
  static constexpr std::uint8_t temp_register = 0x00;
  static constexpr std::uint16_t soft_reset = 0x0222;
  const int wait_for_soft_reset{2};  // milliseconds (datasheet Table 7.6)
  static constexpr std::uint16_t measurement =
      0x0c00;  // no averaging, single measurement
  const int wait_for_measurement{16};
};

#endif