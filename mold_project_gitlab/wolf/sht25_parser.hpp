#ifndef WOLF_SHT25_PARSER_HPP
#define WOLF_SHT25_PARSER_HPP

#include <boost/noncopyable.hpp>
#include "log/logger.hpp"
#include "wolf/sensor_type.hpp"

namespace wolf {

// TODO integrate sensor_id or do a *_handler
class sht25_parser : boost::noncopyable {
 public:
  using read_buffer = std::array<std::uint8_t, 3>;

  sht25_parser();
  float parse(const sensor_sht25_channel channel, const read_buffer& data);

 private:
  float parse_read_temperature(const read_buffer& temperature);
  float parse_read_humidity(const read_buffer& humidity);

 private:
  logging::logger m_logger;
};
}  // namespace wolf

#endif  // WOLF_SHT25_PARSER_HPP
