#include "handler.hpp"

using namespace mold::sync::sensors_values::write;

handler::handler(cache &cache_, sender &sender_,
                 wolf::sensor_value_handler &sensor_values,
                 wolf::login_handler &login)
    : m_logger{"sync::sensors_values::write::handler"},
      m_cache(cache_),
      m_sender(sender_),
      m_sensor_values(sensor_values),
      m_login(login) {
  connect_signals();
}

void handler::resync() {
  try {
    resync_some();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to resync, error:" << error.what();
  }
}

void handler::connect_signals() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "connect_signals";
  m_signal_connections.push_back(m_sensor_values.signal_values.connect(
      [this](const auto &values) { handle(values); }));
}

void handler::resync_some() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "resync_some";
  if (!m_login.is_logged_in()) return;
  const auto to_send = m_cache.get_some();
  if (to_send.empty()) return;
  std::vector<wolf::sensor_value> values;
  std::transform(to_send.cbegin(), to_send.cend(), std::back_inserter(values),
                 [](const auto &item) { return item.value; });
  std::vector<cache::key> keys;
  std::transform(to_send.cbegin(), to_send.cend(), std::back_inserter(keys),
                 [](const auto &item) { return item.key_; });
  m_sender.send(values, [this, keys](const auto &error) {
    handle_resync_response(error, keys);
  });
}

void handler::handle_resync_response(const std::optional<std::string> &error,
                                     const std::vector<cache::key> &keys) {
  if (error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not sync, error:" << error.value();
    return;
  }
  try {
    m_cache.remove(keys);
    resync_some();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle resync response, error:" << error.what();
  }
}

void handler::handle_response(const std::optional<std::string> &error,
                              const wolf::sensor_values &values) {
  if (!error) return;
  std::stringstream error_log;
  error_log << "the server responded with an error for a value, error:"
            << error.value() << ", values:{";
  for (const auto &value : values) error_log << value;
  error_log << "}";
  LOGGING_LOG(m_logger, logging::severity::warning) << error_log.str();
  try {
    handle_offline(values);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "could not handle response, error:" << error.what();
  }
}

static bool shall_get_synced(const wolf::sensor_value &value) {
  return !value.id.has_owner();
}

void handler::handle(const wolf::sensor_values &values) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle, value:" << values;
  try {
    wolf::sensor_values to_sync;
    std::copy_if(values.cbegin(), values.cend(), std::back_inserter(to_sync),
                 shall_get_synced);
    if (to_sync.empty()) return;
    if (!m_login.is_logged_in()) {
      handle_offline(to_sync);
      return;
    }
    handle_online(to_sync);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle a value, error:" << error.what();
  }
}

void handler::reset_values_to_send() {
  LOGGING_LOG(m_logger, logging::severity::normal) << "reset_values_to_send";
  try {
    m_cache.clear_all_values_and_block_new_ones();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to reset values to send, error:" << error.what();
    return;
  }
}

void handler::handle_sensors_to_remove(
    const handler::sensors &sensors_, const std::vector<wolf::room> &all_rooms,
    const wolf::outdoor_sensor &outdoor_sensor) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "check which sensors to delete values for, sensor count: "
      << sensors_.size();
  sensors to_remove = sensors_;
  for (const auto &to_check_room : all_rooms)
    for (const auto &to_check_sensor : to_check_room.sensors) {
      const auto new_end = std::remove_if(
          to_remove.begin(), to_remove.end(),
          [&to_check_sensor, &outdoor_sensor](const wolf::sensor_id &sensor) {
            return sensor == to_check_sensor ||
                   sensor == outdoor_sensor.temperature ||
                   sensor == outdoor_sensor.humidity;
          });
      to_remove.erase(new_end, to_remove.end());
    }
  remove_all_values_for_sensors(to_remove);
}

void handler::handle_offline(const wolf::sensor_values &values) {
  for (const auto &value : values) m_cache.add(value);
}

void handler::handle_online(const wolf::sensor_values &values) {
  m_sender.send(values, [this, values](const auto &error) {
    handle_response(error, values);
  });
}

void handler::remove_all_values_for_sensors(
    const mold::sync::sensors_values::write::handler::sensors &sensors_) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "remove values for " << sensors_.size() << " sensors";
  for (const auto &sensor : sensors_)
    m_cache.remove_all_values_for_sensor_id(sensor);
}
