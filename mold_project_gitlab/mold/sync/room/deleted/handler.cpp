#include "handler.hpp"

using namespace mold::sync::room::deleted;

handler::handler(mold::sync::room::cache &cache_, sender &sender_,
                 wolf::room_handler &rooms, wolf::login_handler &login)
    : m_logger{"sync::room::deleted::handler"},
      m_cache(cache_),
      m_sender(sender_),
      m_rooms(rooms),
      m_login(login) {
  connect_signals();
}

void handler::resync() {
  try {
    LOGGING_LOG(m_logger, logging::severity::verbose) << "resync";
    if (!m_login.is_logged_in()) return;
    const auto to_sync = m_cache.get_deleted();
    for (const auto &sync_ : to_sync) sync(sync_);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to 'resync', what():" << error.what();
  }
}

void handler::connect_signals() {
  m_signal_connections.emplace_back(m_rooms.signal_removed.connect(
      [this](const wolf::types::uuid_array &room) {
        handle_room_removed(room);
      }));
}

void handler::mark_deleted(const wolf::types::uuid_array &room_id) {
  try {
    m_cache.mark_deleted(room_id);
    resync();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to 'mark_deleted', what():" << error.what();
  }
}

void handler::sync(const wolf::types::uuid_array &room_id) {
  m_sender.send(room_id, [this, room_id](const auto &error) {
    handle_response(error, room_id);
  });
}

void handler::handle_room_removed(const wolf::types::uuid_array &room_id) {
  mark_deleted(room_id);
}

void handler::handle_response(const std::optional<std::string> &error,
                              const wolf::types::uuid_array &room) {
  if (error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "handle_response_dirty failed, error:" << error.value();
    return;
  }
  try {
    m_cache.deleted(room);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "fail in 'handle_response_dirty', what():" << error.what();
  }
}
