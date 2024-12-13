#include "sensor_value_reader.hpp"

#include <stdexcept>

sensor_value_reader::sensor_value_reader(const int i2c_address_)
    : i2c_address(i2c_address_){}

bool sensor_value_reader::sensor_value_init() {
  if (!Wire.begin(SDA_Pin, SCL_Pin)) return false;
  if (!do_soft_reset()) return false;
  return get_id_register();
}

double sensor_value_reader::handle_read(const double offset) {
  if (!send_command(measurement))
    throw std::runtime_error("Could not send measurement command!");

  delay(wait_for_measurement);

  if (!send_register_pointer(temp_register))
    throw std::runtime_error("Could not send temperature register!");

  std::array<std::uint8_t, 2> temperature_buffer;
  if (!read_i2c(temperature_buffer.begin(), temperature_buffer.size()))
    throw std::runtime_error("Could not read temperature bytes!");

  int temperature_casted{};
  temperature_casted |= (int{temperature_buffer[0]} << 8);
  temperature_casted |= (int{temperature_buffer[1]} << 0);
  const double sensor_value = static_cast<double>(temperature_casted) * 0.0078125 + offset;

  return sensor_value;
}

bool sensor_value_reader::do_soft_reset() {
  const bool result = send_command(soft_reset);
  delay(wait_for_soft_reset);
  if (!result) throw std::runtime_error("Could not do a soft reset!");
  return true;
}

bool sensor_value_reader::get_id_register() {
  if (!send_register_pointer(id_register)) return false;
  std::array<std::uint8_t, 2> id_buffer;

  if (!read_i2c(id_buffer.begin(), id_buffer.size()))
    throw std::runtime_error("Could not get ID register!");
  return true;
}

bool sensor_value_reader::send_command(const std::uint16_t command) {
  std::array<std::uint8_t, 3> to_send;
  to_send[0] = config_register;
  to_send[1] = (command >> 8) & 0xFF;
  to_send[2] = (command >> 0) & 0xFF;
  return send_i2c(to_send.begin(), to_send.size());
}

bool sensor_value_reader::send_register_pointer(
    const std::uint8_t register_pointer) {
  std::array<std::uint8_t, 1> to_send;
  to_send[0] = register_pointer;
  return send_i2c(to_send.begin(), to_send.size());
}

bool sensor_value_reader::send_i2c(const std::uint8_t* to_send,
                                   const std::size_t size) {
  Wire.beginTransmission(i2c_address);
  const auto sent = Wire.write(to_send, size);
  const auto error = Wire.endTransmission(true);
  if (error == 2)
    throw std::runtime_error("NACK while sending address!");
  else if (error == 3)
    throw std::runtime_error("NACK while sending data!");
  else if (error == 4)
    throw std::runtime_error("Error while sending!");
  return sent == size;
}

bool sensor_value_reader::read_i2c(std::uint8_t* to_read,
                                   const std::size_t size) {
  if (Wire.requestFrom(static_cast<std::uint16_t>(i2c_address), size, true) != size)
    throw std::runtime_error("Could not request necessary data!");
  auto read_counter = 0;
  while (Wire.available()) {
    to_read[read_counter++] = Wire.read();
  }
  // check if all requested data could be read
  if (read_counter < size)
    throw std::runtime_error("Not all requested data could be read!");
  return true;
}

