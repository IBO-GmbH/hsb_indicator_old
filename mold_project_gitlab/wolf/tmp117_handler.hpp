#ifndef WOLF_TMP117_HANDLER_HPP
#define WOLF_TMP117_HANDLER_HPP

#include "program_options_parser.hpp"
#include "sensor_filter.hpp"
#include "sensor_value.hpp"
#include "tmp117_parser.hpp"
#include "tmp117_reader.hpp"

namespace wolf {

class tmp117_handler {
 public:
  using read_buffer = std::array<std::uint8_t, 6>;
  using seconds = std::chrono::seconds;
  using values_type = std::vector<sensor_value>;

  tmp117_handler(tmp117_reader &reader, const wolf::config &program_options);

  void start_reading();
  void resend_signals();

  boost::signals2::signal<void(const values_type &)> signal_values;

 private:
  void handle_read(const types::data &data);
  bool filter_temperature(float &temperature);

 private:
  logging::logger m_logger;
  tmp117_reader &m_reader;
  tmp117_parser m_parser;

  const config &m_program_options;

  sensor_filter m_filter_handler;
  low_pass_filter m_filter_temperature;
};
}  // namespace wolf

#endif  // WOLF_TMP117_HANDLER_HPP
