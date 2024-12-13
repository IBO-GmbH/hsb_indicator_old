#include "display_handler.hpp"
#include <boost/process.hpp>
#include <numeric>

using namespace logging;

mold::display_handler::display_handler(
    const mold::config &program_options, boost::asio::io_service &io_service,
    configuration_state_handler &state_handler)
    : m_logger("display_handler"),
      m_config(program_options),
      m_state_handler(state_handler),
      m_timer(io_service) {
  set_on(true);
  handle_states_update();
}

void mold::display_handler::enable_states_update(const bool enable) {
  if (m_states_update_enabled != enable) m_states_update_enabled = enable;
}

void mold::display_handler::set_on(const bool on) {
  if (on) {
    display_on();
    return;
  }
  display_off();
}

bool mold::display_handler::display_is_locked() const {
  return m_display_locked;
}

void mold::display_handler::set_lock_power_off(const bool disabled) {
  if (m_display_locked == disabled) return;
  change_display_locked(disabled);
  if (!m_display_locked) {
    start_timer();
    return;
  }
  if (m_states_update_enabled) set_on(true);
  m_timer.cancel();
}

void mold::display_handler::handle_states_update() {
  if (!m_states_update_enabled) return;
  // check if we got a yellow/red configuration
  bool all_green_or_confirmed_ = all_green_or_confirmed();
  set_lock_power_off(!all_green_or_confirmed_);
}

void mold::display_handler::handle_co2_state_update(
    const mold::calculation_state &state) {
  set_lock_power_off(state != calculation_state::green);
}

void mold::display_handler::start_timer() {
  if (m_config.display_.timeout <= 0) return;
  m_timer.expires_from_now(std::chrono::seconds(m_config.display_.timeout));
  m_timer.async_wait(
      [this](const boost::system::error_code &error) { handle_wait(error); });
}

void mold::display_handler::display_on() {
  if (!m_on) execute_script(m_config.display_.on_script);
  change_on(true);
  if (!m_display_locked) start_timer();
}

void mold::display_handler::display_off() {
  LOGGING_LOG(m_logger, severity::verbose) << "shutting display off";
  if (m_on) execute_script(m_config.display_.off_script);
  change_on(false);
}

void mold::display_handler::execute_script(const std::string &script) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "executeScript, script:" << script;

  boost::filesystem::path file(script);
  if (!boost::filesystem::exists(file)) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not execute script: " << script << ", file does not exist";
    return;
  }

  const int exit_code = boost::process::system(file);
  if (exit_code != 0) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not execute script: " << script
        << ", exit code = " << std::to_string(exit_code);
    return;
  }
}

void mold::display_handler::change_on(const bool on) {
  if (m_on == on) return;
  m_on = on;
  signal_on(m_on);
}

void mold::display_handler::change_display_locked(const bool display_locked) {
  if (m_display_locked == display_locked) return;
  m_display_locked = display_locked;
}

void mold::display_handler::handle_wait(
    const boost::system::error_code &error) {
  if (error) {
    if (error.value() != boost::asio::error::operation_aborted)
      LOGGING_LOG(m_logger, severity::warning)
          << "timer timed out with error, message:" << error.message();
    return;
  }

  display_off();
}

bool mold::display_handler::all_green_or_confirmed() {
  auto states = m_state_handler.get_all();
  bool all_green_ = std::accumulate(
      states.cbegin(), states.cend(), true,
      [](const bool result,
         const configuration_state_handler::all_result::value_type &check) {
        return result && green_or_confirmed(check.second.state);
      });
  return all_green_;
}

bool mold::display_handler::green_or_confirmed(
    const mold::calculation_state &state) {
  return state == calculation_state::green ||
         state == calculation_state::yellow_confirmed ||
         state == calculation_state::red_confirmed;
}
