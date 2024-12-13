#ifndef WOLF_EE895_PARSER_HPP
#define WOLF_EE895_PARSER_HPP

#include "log/logger.hpp"

namespace wolf {

class ee895_parser {
 public:
  using read_buffer = std::array<std::uint8_t, 8>;
  using value_buffer = std::array<std::uint8_t, 2>;

  struct values {
    float co2;
    float temperature;
    float air_pressure;
  };

  ee895_parser();
  values parse(const read_buffer& data) const;

 private:
  float calculate_co2(const value_buffer& co2) const;
  float calculate_temperature(const value_buffer& temperature) const;
  float calculate_air_pressure(const value_buffer& air_pressure) const;

 private:
  mutable logging::logger m_logger;
};
}  // namespace wolf

#endif  // WOLF_EE895_PARSER_HPP
