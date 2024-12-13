#ifndef WOLF_SHTC1_HANDLER_HPP
#define WOLF_SHTC1_HANDLER_HPP

#include "program_options_parser.hpp"
#include "sensor_filter.hpp"
#include "sensor_value.hpp"
#include "shtc1_parser.hpp"
#include "shtc1_reader.hpp"

namespace wolf {

class shtc1_handler {
 public:
  using read_buffer = std::array<std::uint8_t, 6>;
  using seconds = std::chrono::seconds;
  using values_type = std::vector<sensor_value>;

  shtc1_handler(shtc1_reader &reader, const wolf::config &program_options);
  void start_reading();
  void resend_signals();

  boost::signals2::signal<void(const values_type &)> signal_values;

 private:
  void handle_read(const types::data &data);
  bool filter_temperature(float &temperature);
  bool filter_humidity(float &humidity);

 private:
  logging::logger m_logger;
  shtc1_reader &m_reader;
  shtc1_parser m_parser;

  const config &m_program_options;

  sensor_filter m_filter_handler;
  low_pass_filter m_filter_temperature;
  low_pass_filter m_filter_humidity;
};
}  // namespace wolf

#endif  // WOLF_SHTC1_HANDLER_HPP
