#include "sht85_parser.hpp"
#include "sensirion/crc.hpp"

wolf::sht85_parser::sht85_parser() : m_logger("sht85_parser") {}

wolf::sht85_parser::values wolf::sht85_parser::parse(
    const wolf::sht85_parser::read_buffer &data) const {
  values values_;
  values_.temperature = calculate_temperature({data[0], data[1], data[2]});
  values_.humidity = calculate_humidity({data[3], data[4], data[5]});
  return values_;
}

std::optional<float> wolf::sht85_parser::calculate_temperature(
    const wolf::sht85_parser::value_buffer &temperature) const {
  if (temperature[2] != sensirion::crc::from_carray(temperature.data(), 2)) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "failed to parse temperature, invalid CRC";
    return {};
  }

  int temperature_casted{};
  temperature_casted |= (int{temperature[0]} << 8);
  temperature_casted |= (int{temperature[1]} << 0);
  const float temperature_result =
      (-45.f) +
      175.f * (static_cast<float>(temperature_casted) / (65536.f - 1.f));
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "parsed sht85 temperature:" << temperature_result;
  return temperature_result;
}

std::optional<float> wolf::sht85_parser::calculate_humidity(
    const wolf::sht85_parser::value_buffer &humidity) const {
  if (humidity[2] != sensirion::crc::from_carray(humidity.data(), 2)) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "failed to parse humidity, invalid CRC";
    return {};
  }

  int humidity_casted{};
  humidity_casted |= (humidity[0] << 8);
  humidity_casted |= (humidity[1] << 0);
  const float humidity_result =
      100.f * (static_cast<float>(humidity_casted) / (65536.f - 1.f));
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "parsed sht85 humidity:" << humidity_result;
  return humidity_result;
}
