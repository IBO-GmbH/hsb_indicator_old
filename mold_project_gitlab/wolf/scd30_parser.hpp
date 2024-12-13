#ifndef WOLF_SCD30_PARSER_HPP
#define WOLF_SCD30_PARSER_HPP

#include <array>
#include "log/logger.hpp"

namespace wolf {

class scd30_parser {
 public:
  scd30_parser();

  using data = std::array<std::uint8_t, 18>;
  struct values {
    float co2;
    float temperature;
    float humidity;
  };

  std::optional<values> handle_data(const data& buffer) const;
  using get_data_ready_status = std::array<std::uint8_t, 3>;
  bool parse_get_data_ready_status(get_data_ready_status status) const;

 private:
  static std::optional<float> convert_value(data::const_iterator index);

  mutable logging::logger m_logger;
};

}  // namespace wolf

#endif  // WOLF_SCD30_PARSER_HPP
