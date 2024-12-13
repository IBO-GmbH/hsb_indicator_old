#ifndef WOLF_SCD30_HANDLER_HPP
#define WOLF_SCD30_HANDLER_HPP

#include "log/logger.hpp"
#include "sensor_filter.hpp"
#include "sensor_value.hpp"
#include "wolf/program_options_parser.hpp"
#include "wolf/scd30_parser.hpp"
#include "wolf/scd30_reader.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

namespace wolf {

class scd30_handler {
 public:
  using duration = std::chrono::steady_clock::duration;
  using seconds = std::chrono::seconds;
  using values_type = std::vector<sensor_value>;

  scd30_handler(boost::asio::io_service& service, scd30_reader& reader,
                const config& program_options);
  bool start_reading(const std::optional<sensor_type> second_sensor);
  void close();
  void resend_signals();

  boost::signals2::signal<void(const values_type&)> signal_values;

 private:
  void read_in_interval(const duration& interval);
  void read();
  bool trigger_continous_measurement();
  bool activate_asc();
  bool filter_temperature(float& temperature);
  bool filter_humidity(float& humidity);
  bool filter_co2(float& co2);
  void start_wait_timer();
  void handle_wait_timer_expired(const boost::system::error_code& error);
  bool use_temperature() const;
  bool use_humidity() const;
  bool use_co2() const;

 private:
  mutable logging::logger m_logger;
  boost::asio::steady_timer m_timer_read;
  boost::asio::steady_timer m_timer_co2;
  scd30_reader& m_reader;
  scd30_parser m_parser;
  const config& m_program_options;

  sensor_filter m_filter_handler;
  low_pass_filter m_filter_temperature;
  low_pass_filter m_filter_humidity;
  low_pass_filter m_filter_co2;

  std::optional<sensor_type> m_second_sensor;

  const duration m_value_interval =
      seconds(m_program_options.scd30_.read_interval);
  const seconds start_interval{2};
  const duration co2_wait_time =
      seconds(m_program_options.scd30_.wait_time_co2);

  bool m_wait_timer_expired;
  const bool m_asc_active;
};
}  // namespace wolf

#endif  // WOLF_SCD30_HANDLER_HPP
