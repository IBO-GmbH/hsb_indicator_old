#ifndef WOLF_SHT25_HANDLER_HPP
#define WOLF_SHT25_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "wolf/low_pass_filter_creator.hpp"
#include "wolf/sensor_value.hpp"
#include "wolf/sht25_parser.hpp"
#include "wolf/types.hpp"

namespace wolf {

class sht25_handler {
 public:
  using read_buffer = std::array<std::uint8_t, 3>;

  sht25_handler(const low_pass_filter_creator& filter_creator_temperature,
                const low_pass_filter_creator& filter_creator_humidity);
  ~sht25_handler();

  void handle_read(const sensor_sht25_channel channel, const read_buffer& data);
  void handle_read(const sensor_sht25_channel channel, const types::data& data);
  void resend_signals();

  boost::signals2::signal<void(const sensor_value&)> signal_value;

 private:
  low_pass_filter& get_smoother(const sensor_sht25_channel channel);

  logging::logger m_logger;
  low_pass_filter m_filter_temperature;
  low_pass_filter m_filter_humidity;
  wolf::sht25_parser m_parser;
};
}  // namespace wolf

#endif  // SHWOLF_T25_HANDLER_HPP
