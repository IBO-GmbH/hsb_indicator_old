#include "resync_after_timeout.hpp"

using namespace mold::sync;

resync_after_timeout::resync_after_timeout(
    boost::asio::io_service &service, wolf::login_handler &login,
    box_settings::write::handler &box_settings_,
    room::deleted::handler &deleted,
    room::settings::with_sensors::handler &room_,
    sensors_values::write::handler &values, mold_sprout::handler &mold_sprout_,
    const std::chrono::seconds &timeout)
    : m_logger{"sync::resync_after_timeout"},
      m_timer(service),
      m_login(login),
      m_box_settings(box_settings_),
      m_deleted(deleted),
      m_room(room_),
      m_values(values),
      m_mold_sprout(mold_sprout_),
      m_timeout(timeout) {
  connect_signals();
}

void resync_after_timeout::connect_signals() {
  m_connection_login = m_login.signal_logged_in.connect(
      [this](const auto logged_in) { handle_logged_in_with_catch(logged_in); });
}

void resync_after_timeout::handle_logged_in_with_catch(const bool logged_in) {
  try {
    handle_logged_in(logged_in);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle_logged_in failed. what():" << error.what();
  }
}

void resync_after_timeout::handle_logged_in(const bool logged_in_) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle_logged_in, logged_in:" << logged_in_;
  if (logged_in_) {
    logged_in();
    return;
  }
  logged_out();
}

void resync_after_timeout::logged_in() { start_timer(); }

void resync_after_timeout::start_timer() {
  m_timer.expires_from_now(m_timeout);
  m_timer.async_wait(
      [this](const auto &error) { handle_timeout_with_catch(error); });
}

void resync_after_timeout::logged_out() { m_timer.cancel(); }

void resync_after_timeout::handle_timeout_with_catch(
    const boost::system::error_code &error) {
  try {
    handle_timeout(error);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle_timeout failed. what():" << error.what();
  }
}

void resync_after_timeout::handle_timeout(
    const boost::system::error_code &error) {
  if (error.value() == boost::asio::error::operation_aborted) return;
  if (error) throw boost::system::system_error(error);
  m_box_settings.resync();
  m_room.resync();
  m_deleted.resync();
  m_values.resync();
  m_mold_sprout.resync();
  start_timer();
}
