#ifndef MOLD_COMFORT_QUESTIONNAIRE_HANDLER_HPP
#define MOLD_COMFORT_QUESTIONNAIRE_HANDLER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/signals2/signal.hpp>
#include "comfort_questionnaire_cache.hpp"
#include "wolf/sensor_id.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace mold {

class comfort_questionnaire_handler {
 public:
  comfort_questionnaire_handler(comfort_questionnaire_cache *const cache,
                                wolf::sensor_value_handler &value_handler,
                                boost::asio::io_service &io_service);

  void set_options(const questionnaire_options &options);
  questionnaire_options get_options();
  void deactivate_questionnaire();
  void close();
  // questionnaire should be delayed for 30 minutes because later was pressed
  void delay_questionnaire();
  void save_questionnaire(const questionnaire &to_save);
  void discard_questionnaire();
  void add_sensors_if_necessary();
  std::optional<questionnaire> get_last_questionnaire();

  boost::signals2::signal<void()> signal_cancel_last_questionnaire;
  boost::signals2::signal<void()> signal_open_next_questionnaire;
  using questionnaire_sensors = std::vector<wolf::sensor_id>;
  boost::signals2::signal<void(const bool, const questionnaire_sensors &)>
      signal_change_questionnaire_sensors;

 private:
  void change_activated_status(const bool status);
  void set_cancel_timer();
  void restart_cancel_timer();
  void start_cancel_timer(const boost::system::error_code &error,
                          const std::string &error_text,
                          const std::tm &next_time);
  void handle_wait_cancel_timer(const boost::system::error_code &error);
  void set_next_questionnaire_timer(const int hours, const int minutes,
                                    const bool next_day);
  void cancel_all_timers();
  void calculate_next_questionnaire_time(
      const std::chrono::system_clock::time_point &time);
  static std::tm *convert_timepoint_to_tm(
      const std::chrono::system_clock::time_point &time);
  void handle_wait_next_questionnaire_timer(
      const boost::system::error_code &error);
  void start_next_questionnaire_timer(const boost::system::error_code &error);
  void create_sensors();
  void add_sensors();
  void remove_sensors();
  using sensor_values = std::vector<wolf::sensor_value>;
  sensor_values convert_questionnaire_to_sensor_values(
      const questionnaire &to_convert);

 private:
  logging::logger m_logger;
  comfort_questionnaire_cache *const m_cache;
  wolf::sensor_value_handler &m_value_handler;
  // after 22:00 the questionnaire on the screen shall be discarded
  boost::asio::deadline_timer m_cancel_last_questionnaire_timer;
  boost::asio::deadline_timer m_open_next_questionnaire_timer;

  questionnaire_options m_options;
  questionnaire_sensors m_sensors;
  std::optional<questionnaire> m_last_questionnaire;
  static constexpr questionnaire_options default_options{
      false,
      3,
      {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };
};
}  // namespace mold

#endif  // MOLD_COMFORT_QUESTIONNAIRE_HANDLER_HPP
