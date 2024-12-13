#include "handler.hpp"
#include "mold/sprout/latest_value_helper.hpp"

using namespace mold::sync::mold_sprout;

handler::handler(cache &cache_, sender &sender_, mold::sprout::handler &sprout,
                 wolf::login_handler &login,
                 configuration_handler &configurations)
    : m_logger{"sync::mold_sprout::handler"},
      m_cache(cache_),
      m_sender(sender_),
      m_sprout(sprout),
      m_login(login),
      m_configurations(configurations) {
  connect_signals();
}

void handler::resync() {
  if (!m_login.is_logged_in()) return;
  const auto to_send = m_cache.get_all();
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "resync, count:" << to_send.size();
  for (const auto &item : to_send) send(item.room_id, item.value, item.rowid);
}

void handler::connect_signals() {
  m_signal_connections.push_back(m_sprout.signal_sprout_type_changed.connect(
      [this](const auto &value) { handle_with_catch(value); }));
}

void handler::handle_with_catch(const mold::sprout::data_types::value &value) {
  try {
    handle(value);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle, error:" << error.what();
  }
}

void handler::handle(const mold::sprout::data_types::value &value) {
  const auto configuration_id = value.configuration;
  const auto latest_value = sprout::latest_value_helper{value}();
  const auto configuration = m_configurations.get_or_throw(configuration_id);
  const auto room_id = configuration.room;
  const int rowid = m_cache.add(room_id, latest_value);
  send(room_id, latest_value, rowid);
}

void handler::send(const wolf::types::uuid_array &room_id,
                   const mold::sprout_value &value, int rowid) {
  const auto callback = [rowid, this](const auto &error) {
    handle_confirmed(error, rowid);
  };
  m_sender.send({room_id, value}, callback);
}

void handler::handle_confirmed(const std::optional<std::string> &error,
                               int rowid) {
  if (error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle_confirmed called with error:" << error.value();
    return;
  }
  m_cache.remove(rowid);
}
