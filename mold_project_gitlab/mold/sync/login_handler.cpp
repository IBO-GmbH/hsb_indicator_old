#include "login_handler.hpp"

using namespace mold::sync;

login_handler::login_handler(wolf::login_handler &login,
                             box_settings::write::handler &box_settings_,
                             room::deleted::handler &deleted,
                             room::settings::with_sensors::handler &room_,
                             sensors_values::write::handler &values,
                             mold_sprout::handler &mold_sprout_)
    : m_logger{"sync::resync_after_timeout"},
      m_login(login),
      m_box_settings(box_settings_),
      m_deleted(deleted),
      m_room(room_),
      m_values(values),
      m_mold_sprout(mold_sprout_) {
  connect_signals();
}

void login_handler::connect_signals() {
  m_connection_login = m_login.signal_logged_in.connect(
      [this](const auto logged_in) { handle_logged_in_with_catch(logged_in); });
}

void login_handler::handle_logged_in_with_catch(const bool logged_in) {
  try {
    handle_logged_in(logged_in);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle_logged_in failed. what():" << error.what();
  }
}

void login_handler::handle_logged_in(const bool logged_in_) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle_logged_in, logged_in:" << logged_in_;
  if (!logged_in_) return;
  m_box_settings.resync();
  m_room.resync();
  m_deleted.resync();
  m_values.resync();
  m_mold_sprout.resync();
}
