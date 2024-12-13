#include "scd30_parser.hpp"
#include "sensirion/uint16_parser.hpp"

using namespace wolf;

scd30_parser::scd30_parser() : m_logger{"scd30_parser"} {}

std::optional<scd30_parser::values> scd30_parser::handle_data(
    const scd30_parser::data &buffer) const {
  auto current = buffer.cbegin();
  const auto co2 = convert_value(current);
  std::advance(current, 6);
  auto temperature = convert_value(current);
  std::advance(current, 6);
  auto humidity = convert_value(current);

  // manual offset
  if (temperature.has_value()) temperature.value() -= 1.3f;
  if (humidity.has_value()) humidity.value() += 3.f;

  if (!co2.has_value() || !temperature.has_value() || !humidity.has_value()) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "failed to parse buffer";
    return {};
  }

  const values result = {*co2, *temperature, *humidity};
  return result;
}

bool scd30_parser::parse_get_data_ready_status(
    get_data_ready_status status) const {
  const auto result = sensirion::uint16_parser::check_crc8_and_cast(status);
  return result == 1;
}

std::optional<float> scd30_parser::convert_value(data::const_iterator index) {
  sensirion::uint16_parser::data_type data;
  std::copy_n(index, data.size(), data.begin());
  const auto first = sensirion::uint16_parser::check_crc8_and_cast(data);
  if (!first.has_value()) return {};

  std::copy_n(std::next(index, 3), data.size(), data.begin());
  const auto second = sensirion::uint16_parser::check_crc8_and_cast(data);
  if (!second.has_value()) return {};
  const int to_cast = (*first << 16) | *second;
  float result;
  static_assert(sizeof(float) == sizeof(int),
                "float and int are not the same size");
  std::memcpy(&result, &to_cast, sizeof(to_cast));
  return result;
}
