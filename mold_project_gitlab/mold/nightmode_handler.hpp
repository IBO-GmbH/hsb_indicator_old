#ifndef MOLD_NIGHTMODE_HANDLER_HPP
#define MOLD_NIGHTMODE_HANDLER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/signals2/signal.hpp>
#include <chrono>
#include "log/logger.hpp"
#include "mold/display_handler_base.hpp"
#include "mold/program_options_parser.hpp"
#include "mold/tone_enabled_handler.hpp"

namespace mold {

class nightmode_handler {
 public:
  using time_point = std::chrono::system_clock::time_point;
  using duration = std::chrono::system_clock::duration;

  nightmode_handler(const mold::config &program_options,
                    mold::options_cache *const database,
                    mold::display_handler_base *display_handler,
                    boost::asio::io_service &io_service);
  void enable();
  void disable();
  bool is_enabled() const;
  void set_active(const bool active);
  bool is_active() const;
  bool check_if_active(const time_point &time_to_check);
  void close();
  std::time_t calculate_time(const time_point &time_to_check,
                             const int check_hours,
                             const int check_minutes) const;

  boost::signals2::signal<void(const bool)> signal_enabled;
  boost::signals2::signal<void(const bool)> signal_active;

 private:
  bool check(const time_point &time_to_check);
  bool check_time(const time_point &time_to_check) const;
  bool handle_check(const time_point &time_to_check);
  void set_enabled(const bool enable);
  void cancel_timer();
  void handle_wait(const boost::system::error_code &error,
                   const time_point &time_to_check);

 private:
  mutable logging::logger m_logger;
  const mold::config &m_program_options;
  mold::options_cache *const m_database;
  mold::display_handler_base *m_display_handler;
  boost::asio::deadline_timer m_check_timer;

  bool m_enabled;
  bool m_active;
};
}  // namespace mold

#endif  // MOLD_NIGHTMODE_HANDLER_HPP
