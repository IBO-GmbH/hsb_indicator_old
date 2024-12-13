#ifndef WOLF_TMP117_PARSER_HPP
#define WOLF_TMP117_PARSER_HPP

#include "log/logger.hpp"

namespace wolf {

class tmp117_parser {
 public:
  using read_buffer = std::array<std::uint8_t, 2>;

  struct values {
    std::optional<float> temperature;
  };

  tmp117_parser();

  values parse(const read_buffer& data) const;

 private:
  std::optional<float> calculate_temperature(
      const read_buffer& temperature) const;

 private:
  mutable logging::logger m_logger;
};
}  // namespace wolf

#endif  // WOLF_TMP117_PARSER_HPP
