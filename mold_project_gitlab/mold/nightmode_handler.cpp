#include "nightmode_handler.hpp"

#include "mold/options_cache.hpp"
#include "wolf/thread_checker.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>

using namespace logging;

mold::nightmode_handler::nightmode_handler(
    const mold::config &program_options, options_cache *database,
    mold::display_handler_base *display_handler,
    boost::asio::io_service &io_service)
    : m_logger("nightmode_handler"),
      m_program_options(program_options),
      m_database(database),
      m_display_handler(display_handler),
      m_check_timer(io_service),
      m_enabled(false),
      m_active(false) {
  if (m_database)
    m_enabled = m_database->get_nightmode_enabled().get_value_or(false);
}

void mold::nightmode_handler::enable() {
  LOGGING_LOG(m_logger, severity::verbose) << "nightmode enabled";
  wolf::thread_checker::throw_if_not_service("nightmode_handler::enable");
  set_enabled(true);
}

void mold::nightmode_handler::disable() {
  LOGGING_LOG(m_logger, severity::verbose) << "nightmode disabled";
  wolf::thread_checker::throw_if_not_service("nightmode_handler::disable");
  m_display_handler->enable_states_update(true);
  cancel_timer();
  set_enabled(false);
}

bool mold::nightmode_handler::is_enabled() const { return m_enabled; }

bool mold::nightmode_handler::is_active() const { return m_active; }

bool mold::nightmode_handler::check_if_active(const time_point &time_to_check) {
  if (!is_enabled()) return false;
  return check(time_to_check);
}

void mold::nightmode_handler::close() { cancel_timer(); }

bool mold::nightmode_handler::check(
    const mold::nightmode_handler::time_point &time_to_check) {
  boost::system::error_code error;
  auto time =
      calculate_time(time_to_check, m_program_options.nightmode_.off.hours,
                     m_program_options.nightmode_.off.minutes);
  m_check_timer.expires_at(boost::posix_time::from_time_t(time), error);
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not set expire time to timer, message:" << error.message();
    return false;
  }
  m_check_timer.async_wait(
      [this, time_to_check](const boost::system::error_code &error) {
        handle_wait(error, time_to_check);
      });
  return handle_check(time_to_check);
}

bool mold::nightmode_handler::check_time(
    const mold::nightmode_handler::time_point &time_to_check) const {
  std::time_t time = std::chrono::system_clock::to_time_t(time_to_check);
  std::tm *to_check = std::localtime(&time);
  if (m_program_options.nightmode_.on.hours >
      m_program_options.nightmode_.off.hours)
    if (to_check->tm_hour < m_program_options.nightmode_.on.hours &&
        to_check->tm_hour > m_program_options.nightmode_.off.hours)
      return false;
  if (m_program_options.nightmode_.on.hours <=
      m_program_options.nightmode_.off.hours)
    if (to_check->tm_hour < m_program_options.nightmode_.on.hours ||
        to_check->tm_hour > m_program_options.nightmode_.off.hours)
      return false;
  if (to_check->tm_hour == m_program_options.nightmode_.on.hours &&
      to_check->tm_min < m_program_options.nightmode_.on.minutes)
    return false;
  if (to_check->tm_hour == m_program_options.nightmode_.off.hours &&
      to_check->tm_min >= m_program_options.nightmode_.off.minutes)
    return false;
  return true;
}

bool mold::nightmode_handler::handle_check(
    const mold::nightmode_handler::time_point &time_to_check) {
  bool active = check_time(time_to_check);
  auto display_locked = m_display_handler->display_is_locked();
  if (active) {
    m_display_handler->enable_states_update(false);
    if (display_locked) m_display_handler->set_on(false);
  } else {
    m_display_handler->enable_states_update(true);
    m_display_handler->handle_states_update();
    if (display_locked) m_display_handler->set_on(true);
  }
  set_active(active);
  return active;
}

void mold::nightmode_handler::set_enabled(const bool enable) {
  if (m_enabled == enable) return;
  m_enabled = enable;
  if (m_database) m_database->set_nightmode_enabled(enable);
  signal_enabled(enable);
}

void mold::nightmode_handler::cancel_timer() {
  boost::system::error_code error;
  m_check_timer.cancel(error);
  if (error)
    LOGGING_LOG(m_logger, severity::warning)
        << "could not cancel timer, message:" << error.message();
}

void mold::nightmode_handler::handle_wait(
    const boost::system::error_code &error, const time_point &time_to_check) {
  if (error) {
    if (error.value() != boost::asio::error::operation_aborted)
      LOGGING_LOG(m_logger, severity::warning)
          << "timer timed out with error, message:" << error.message();
    return;
  }

  if (!handle_check(time_to_check)) return;
  check(std::chrono::system_clock::now());
}

void mold::nightmode_handler::set_active(const bool active) {
  if (active == m_active) return;
  m_active = active;
  signal_active(active);
}

time_t mold::nightmode_handler::calculate_time(
    const mold::nightmode_handler::time_point &time_to_check,
    const int check_hours, const int check_minutes) const {
  std::time_t time = std::chrono::system_clock::to_time_t(time_to_check);
  std::tm *to_check = std::localtime(&time);
  // check if it is before midnight and after on/off time
  if (to_check->tm_hour <= 23 && to_check->tm_min <= 59) {
    if (to_check->tm_hour > check_hours) ++to_check->tm_mday;
    if (to_check->tm_hour == check_hours && to_check->tm_min >= check_minutes)
      ++to_check->tm_mday;
  }
  to_check->tm_hour = check_hours;
  to_check->tm_min = check_minutes;
  return std::mktime(to_check);
}
