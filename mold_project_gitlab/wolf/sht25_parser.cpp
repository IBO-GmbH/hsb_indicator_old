#include "sht25_parser.hpp"

using namespace logging;
using namespace wolf;

sht25_parser::sht25_parser() : m_logger("sht25_parser") {}

float sht25_parser::parse(const sensor_sht25_channel channel,
                          const sht25_parser::read_buffer &data) {
  float result;
  if (channel == sensor_sht25_channel::temperature) {
    result = parse_read_temperature(data);
  } else {
    result = parse_read_humidity(data);
  }
  return result;
}

float sht25_parser::parse_read_temperature(
    const sht25_parser::read_buffer &temperature) {
  // TODO check lsb if data right!
  // TODO do crc!
  int temperature_casted{};
  temperature_casted |= (int{temperature[0]} << 8);
  temperature_casted |= (int{temperature[1]} << 0);
  temperature_casted &= ~0b11;
  const float temperature_result =
      (-46.85f) + 175.72f * (static_cast<float>(temperature_casted) / 65536.f) -
      1.3f;  // manual offset
  LOGGING_LOG(m_logger, severity::normal)
      << "parsed sht25 temperature:" << temperature_result;
  return temperature_result;
}

float sht25_parser::parse_read_humidity(
    const sht25_parser::read_buffer &humidity) {
  int humidity_casted{};
  humidity_casted |= (humidity[0] << 8);
  humidity_casted |= (humidity[1] << 0);
  humidity_casted &= ~0b11;
  const float humidity_result =
      (-6.f) + 125.f * (static_cast<float>(humidity_casted) / 65536.f) +
      3.f;  // manual offset
  LOGGING_LOG(m_logger, severity::normal) << "parsed sht25 humidity value:"
                                             " humidity:"
                                          << humidity_result;
  return humidity_result;
}
