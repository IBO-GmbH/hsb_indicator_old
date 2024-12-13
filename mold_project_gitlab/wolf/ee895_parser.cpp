#include "ee895_parser.hpp"

using namespace logging;

wolf::ee895_parser::ee895_parser() : m_logger("ee895_parser") {}

wolf::ee895_parser::values wolf::ee895_parser::parse(
    const wolf::ee895_parser::read_buffer &data) const {
  values values_;
  values_.co2 = calculate_co2({data[0], data[1]});
  values_.temperature = calculate_temperature({data[2], data[3]});
  // byte 5 and 6 are not used
  values_.air_pressure = calculate_air_pressure({data[6], data[7]});
  return values_;
}

float wolf::ee895_parser::calculate_co2(
    const wolf::ee895_parser::value_buffer &co2) const {
  int co2_casted{};
  co2_casted |= (int{co2[0]} << 8);
  co2_casted |= (int{co2[1]} << 0);
  const float co2_result = static_cast<float>(co2_casted);
  LOGGING_LOG(m_logger, severity::verbose)
      << "parsed ee895 co2: " << co2_result;
  return co2_result;
}

float wolf::ee895_parser::calculate_temperature(
    const wolf::ee895_parser::value_buffer &temperature) const {
  int temperature_casted{};
  temperature_casted |= (int{temperature[0]} << 8);
  temperature_casted |= (int{temperature[1]} << 0);
  const float temperature_result =
      static_cast<float>(temperature_casted) / 100.f;
  LOGGING_LOG(m_logger, severity::verbose)
      << "parsed ee895 temperature: " << temperature_result;
  return temperature_result;
}

float wolf::ee895_parser::calculate_air_pressure(
    const wolf::ee895_parser::value_buffer &air_pressure) const {
  int air_pressure_casted{};
  air_pressure_casted |= (int{air_pressure[0]} << 8);
  air_pressure_casted |= (int{air_pressure[1]} << 0);
  const float air_pressure_result =
      static_cast<float>(air_pressure_casted) / 10.f;
  LOGGING_LOG(m_logger, severity::verbose)
      << "parsed ee895 air_pressure:" << air_pressure_result;
  return air_pressure_result;
}
