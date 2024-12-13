#include "tmp117_parser.hpp"

wolf::tmp117_parser::tmp117_parser() : m_logger("tmp117_parser") {}

wolf::tmp117_parser::values wolf::tmp117_parser::parse(
    const wolf::tmp117_parser::read_buffer &data) const {
  values values_;
  values_.temperature = calculate_temperature(data);
  return values_;
}

std::optional<float> wolf::tmp117_parser::calculate_temperature(
    const wolf::tmp117_parser::read_buffer &temperature) const {
  int temperature_casted{};
  temperature_casted |= (int{temperature[0]} << 8);
  temperature_casted |= (int{temperature[1]} << 0);
  const float temperature_result =
      static_cast<float>(temperature_casted) * 0.0078125f;
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "parsed tmp117 temperature:" << temperature_result;
  return temperature_result;
}
