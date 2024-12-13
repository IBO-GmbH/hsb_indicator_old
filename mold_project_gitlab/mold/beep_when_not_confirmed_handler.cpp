#include "beep_when_not_confirmed_handler.hpp"
#include "mold/calculation_state.hpp"

using namespace mold;
using namespace logging;

beep_when_not_confirmed_handler::beep_when_not_confirmed_handler(
    boost::asio::io_service &service, const std::chrono::seconds &beep_interval)
    : m_logger("beep_when_not_confirmed_handler"),
      m_beep_timer(service),
      m_beep_interval(beep_interval) {}

void beep_when_not_confirmed_handler::handle_state(
    const wolf::types::uuid_array &config_id, const calculation_state state) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_state, config_id:" << config_id << " state:" << state;
  const auto must_be_confirmed =
      (state == calculation_state::yellow) || (state == calculation_state::red);

  const auto found =
      std::find(m_configs_that_are_not_confirmed.begin(),
                m_configs_that_are_not_confirmed.end(), config_id);
  // config is listed as not-confirmed
  if (found != m_configs_that_are_not_confirmed.cend()) {
    if (!must_be_confirmed)
      m_configs_that_are_not_confirmed.erase(found);
    else
      return;
  } else {
    // config is not listed as not-confirmed
    if (!must_be_confirmed) return;
    m_configs_that_are_not_confirmed.push_back(config_id);
  }
  if (m_configs_that_are_not_confirmed.empty()) {
    cancel();
    return;
  }
  start_timer();
}

void beep_when_not_confirmed_handler::cancel() {
  boost::system::error_code ignore_error;
  m_beep_timer.cancel(ignore_error);
  if (ignore_error)
    LOGGING_LOG(m_logger, severity::warning)
        << "could not cancel timer, message:" << ignore_error.message();
}

void beep_when_not_confirmed_handler::confirm_all() {
  for (const auto &to_confirm : m_configs_that_are_not_confirmed)
    signal_confirmed(to_confirm);
  m_configs_that_are_not_confirmed.clear();
  cancel();
}

void beep_when_not_confirmed_handler::start_timer() {
  LOGGING_LOG(m_logger, severity::verbose) << "start_timer";

  boost::system::error_code error;
  m_beep_timer.expires_from_now(m_beep_interval, error);
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not set expire time to timer, message:" << error.message();
    return;
  }
  m_beep_timer.async_wait(
      [this](const boost::system::error_code &error) { handle_beep(error); });
}

void beep_when_not_confirmed_handler::handle_beep(
    const boost::system::error_code &error) {
  if (error == boost::asio::error::operation_aborted) return;
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "time timed out with error, message:" << error.message();
    return;
  }

  signal_beep();
  start_timer();
}
