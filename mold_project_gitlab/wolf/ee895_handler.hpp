#ifndef WOLF_EE895_HANDLER_HPP
#define WOLF_EE895_HANDLER_HPP

#include "ee895_parser.hpp"
#include "ee895_reader.hpp"
#include "log/logger.hpp"
#include "program_options_parser.hpp"
#include "sensor_filter.hpp"
#include "sensor_value.hpp"

namespace wolf {

class ee895_handler {
 public:
  using seconds = std::chrono::seconds;
  using values_type = std::vector<sensor_value>;

  ee895_handler(ee895_reader &reader, const wolf::config &program_options);
  void start_reading(const std::optional<sensor_type> second_sensor);
  void resend_signals();

  boost::signals2::signal<void(const values_type &)> signal_values;

 private:
  void handle_read(const ee895_reader::read_buffer &data);
  bool filter_co2(float &co2);
  bool filter_temperature(float &temperature);
  bool filter_air_pressure(float &air_pressure);
  bool use_temperature() const;

 private:
  logging::logger m_logger;
  ee895_reader &m_reader;
  ee895_parser m_parser;

  const config &m_program_options;

  sensor_filter m_filter_handler;
  low_pass_filter m_filter_co2;
  low_pass_filter m_filter_temperature;
  low_pass_filter m_filter_air_pressure;

  std::optional<sensor_type> m_second_sensor;
};
}  // namespace wolf

#endif  // WOLF_EE895_HANDLER_HPP
