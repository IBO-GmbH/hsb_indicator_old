#include "nightmode_activator.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace logging;

mold::nightmode_activator::nightmode_activator(
    const config &program_options, mold::nightmode_handler &nightmode,
    boost::asio::io_service &service)
    : m_logger("nightmode_activator"),
      m_program_options(program_options),
      m_nightmode(nightmode),
      m_timer(service) {
  check_if_enabled();
}

void mold::nightmode_activator::activate(
    const mold::nightmode_activator::time_point &time_to_check) {
  LOGGING_LOG(m_logger, severity::verbose) << "activate nightmode";
  handle_activate(time_to_check);
}

void mold::nightmode_activator::deactivate() {
  LOGGING_LOG(m_logger, severity::verbose) << "deactivate nightmode";
  m_nightmode.set_active(false);
  boost::system::error_code error;
  m_timer.cancel(error);
  if (error)
    LOGGING_LOG(m_logger, severity::warning)
        << "could not cancel timer, message:" << error.message();
}

void mold::nightmode_activator::check_if_enabled() {
  bool enabled = m_nightmode.is_enabled();
  if (!enabled) return;
  activate(std::chrono::system_clock::now());
}

void mold::nightmode_activator::check(
    const mold::nightmode_activator::time_point &time_to_check) {
  const bool active = m_nightmode.check_if_active(time_to_check);
  LOGGING_LOG(m_logger, severity::verbose)
      << "nightmode " << (active ? "active" : "inactive");
}

void mold::nightmode_activator::handle_activate(
    const mold::nightmode_activator::time_point &time_to_check) {
  check(time_to_check);
  auto time = calculate_time(time_to_check);
  boost::system::error_code error;
  m_timer.expires_at(boost::posix_time::from_time_t(time), error);
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not set expire time to timer, message:" << error.message();
    return;
  }
  m_timer.async_wait(
      [this](const boost::system::error_code &error) { handle_wait(error); });
}

time_t mold::nightmode_activator::calculate_time(
    const mold::nightmode_activator::time_point &time_to_check) const {
  auto hours = m_program_options.nightmode_.on.hours;
  auto minutes = m_program_options.nightmode_.on.minutes;
  return m_nightmode.calculate_time(time_to_check, hours, minutes);
}

void mold::nightmode_activator::handle_wait(
    const boost::system::error_code &error) {
  if (error) {
    if (error.value() != boost::asio::error::operation_aborted)
      LOGGING_LOG(m_logger, severity::warning)
          << "timer timed out with error, message:" << error.message();
    return;
  }

  handle_activate(std::chrono::system_clock::now());
}
