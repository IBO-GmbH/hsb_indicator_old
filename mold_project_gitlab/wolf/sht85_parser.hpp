#ifndef WOLF_SHT85_PARSER_HPP
#define WOLF_SHT85_PARSER_HPP

#include "log/logger.hpp"

namespace wolf {

class sht85_parser {
 public:
  using read_buffer = std::array<std::uint8_t, 6>;
  using value_buffer = std::array<std::uint8_t, 3>;

  struct values {
    std::optional<float> temperature;
    std::optional<float> humidity;
  };

  sht85_parser();

  values parse(const read_buffer& data) const;

 private:
  std::optional<float> calculate_temperature(
      const value_buffer& temperature) const;
  std::optional<float> calculate_humidity(const value_buffer& humidity) const;

 private:
  mutable logging::logger m_logger;
};
}  // namespace wolf

#endif  // WOLF_SHT85_PARSER_HPP
