#include "sensor_value_reader.hpp"

#include <stdexcept>

sensor_value_reader::sensor_value_reader()
    : read_interval(0),
      sensor_values_({0.0, 0.0, 0.0}),
      check_time(0),
      offsets({0.0, 0.0, 0.0}),
      first_read(true) {}

void sensor_value_reader::sensor_value_init() {
  Wire.begin();
  check_time = millis();
}

bool sensor_value_reader::handle_read(const int address) {
  auto current_time = millis();
  if (((current_time - check_time) < (read_interval * 1000)) && !first_read)
    return false;
  check_time = current_time;
  first_read = false;

  auto read_counter = 0;
  Wire.beginTransmission(address);
  Wire.write(0);
  Wire.endTransmission(true);
  // read data from slave device
  Wire.requestFrom(address, 8, true);
  while (Wire.available()) {
    i2c_response[read_counter++] = Wire.read();
  }
  // check if all requested data could be read
  if (read_counter < 8)
    throw std::runtime_error("Not all measurement data could be read!");

  sensor_values_[0] =
      static_cast<double>(i2c_response[2] * 256 + i2c_response[3]) / 100 +
      offsets[0];
  sensor_values_[1] =
      static_cast<double>(i2c_response[0] * 256 + i2c_response[1]) + offsets[1];
  sensor_values_[2] =
      static_cast<double>(i2c_response[6] * 256 + i2c_response[7]) / 10 +
      offsets[2];
  return true;
}

sensor_value_reader::sensor_values sensor_value_reader::get_sensor_values() {
  return sensor_values_;
}

void sensor_value_reader::change_read_interval(const unsigned int interval) {
  if (read_interval != interval) read_interval = interval;
}

void sensor_value_reader::change_sensor_offsets(
    const sensor_values& new_offsets) {
  if (offsets != new_offsets) offsets = new_offsets;
}
