#include "resync_everything.hpp"

using namespace mold::sync;

resync_everything::resync_everything(
    wolf::login_handler &login, box_settings::write::handler &box_settings_,
    room::settings::with_sensors::handler &room_)
    : m_logger{"sync::resync_everything"},
      m_login(login),
      m_box_settings(box_settings_),
      m_room(room_) {
  connect_signals();
}

void resync_everything::connect_signals() {
  m_connection_resync =
      m_login.signal_resync.connect([this] { handle_with_catch(); });
}

void resync_everything::handle_with_catch() {
  try {
    handle();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle failed. what():" << error.what();
  }
}

void resync_everything::handle() {
  LOGGING_LOG(m_logger, logging::severity::normal) << "handle";
  m_room.resync_all();
  m_box_settings.resync_all();
}
