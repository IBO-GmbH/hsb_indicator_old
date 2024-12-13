#include "configuration_handler.hpp"
#include <boost/optional/optional_io.hpp>
#include "wolf/thread_checker.hpp"

using namespace mold;
using namespace logging;

configuration_handler::configuration_handler(
    configuration_cache &database, wolf::key_value_cache &key_value_cache)
    : m_logger{"configuration_handler"},
      m_database(database),
      m_key_value_cache(key_value_cache) {
  load_all_from_database();
}

void configuration_handler::add(const configuration &to_add) {
  LOGGING_LOG(m_logger, severity::normal) << "adding, to_add:" << to_add;
  wolf::thread_checker::throw_if_not_service("configuration_handler::add");
  if (get_by_room_id(to_add.room))
    throw std::runtime_error("configuration_handler::add: already found!");
  m_configurations.push_back(to_add);
  m_database.add(to_add);
  signal_added(to_add);
}

void configuration_handler::update(const configuration &to_update) {
  LOGGING_LOG(m_logger, severity::normal)
      << "updating, to_update:" << to_update;
  wolf::thread_checker::throw_if_not_service("configuration_handler::update");
  configurations::iterator it_found =
      std::find_if(m_configurations.begin(), m_configurations.end(),
                   [&to_update](const configuration &config) {
                     return config.id == to_update.id;
                   });
  if (it_found == m_configurations.end()) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not update, because not found!";
    throw std::runtime_error(
        "configuration_handler::update: to update not found!");
  }
  if (*it_found == to_update) {
    LOGGING_LOG(m_logger, severity::normal) << "update, nothing changed";
    return;
  }
  *it_found = to_update;
  m_database.update(to_update);
  signal_updated(to_update);
}

void configuration_handler::remove(const wolf::types::uuid_array &id) {
  LOGGING_LOG(m_logger, severity::normal) << "remove, id:" << id;
  wolf::thread_checker::throw_if_not_service("configuration_handler::remove");
  configurations::iterator it_found = std::find_if(
      m_configurations.begin(), m_configurations.end(),
      [&id](const configuration &config) { return config.id == id; });
  if (it_found == m_configurations.end()) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not remove, because not found!";
    throw std::runtime_error(
        "configuration_handler::remove: to remove not found!");
  }
  m_configurations.erase(it_found);
  m_database.remove(id);
  signal_removed(id);
}

void configuration_handler::set_outdoor_sensor(
    const wolf::outdoor_sensor &sensor_id) {
  wolf::thread_checker::throw_if_not_service(
      "configuration_handler::set_outdoor_sensor");
  LOGGING_LOG(m_logger, severity::verbose)
      << "set_outdoor_sensor, sensor:" << sensor_id;
  auto old_outdoor = m_outdoor_sensor;
  if (m_outdoor_sensor == sensor_id) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "set_outdoor_sensor, m_outdoor_sensor == sensor_id:" << sensor_id;
    return;
  }
  m_outdoor_sensor = sensor_id;

  LOGGING_LOG(m_logger, severity::verbose) << "set_outdoor_sensor, to database";

  m_key_value_cache.set_outdoor_sensor(sensor_id.temperature);
  m_key_value_cache.set_outdoor_sensor_humidity(sensor_id.humidity);
  m_key_value_cache.set_outdoor_sensor_synced(false);

  signal_outdoor_changed(old_outdoor, m_outdoor_sensor);
}

const wolf::outdoor_sensor &configuration_handler::get_outdoor_sensor() const {
  return m_outdoor_sensor;
}

const configuration_handler::configurations &configuration_handler::get_all()
    const {
  wolf::thread_checker::throw_if_not_service("configuration_handler::get_all");
  return m_configurations;
}

boost::optional<configuration> configuration_handler::get(
    const wolf::types::uuid_array &id) const {
  wolf::thread_checker::throw_if_not_service("configuration_handler::get");
  configurations::const_iterator found = std::find_if(
      m_configurations.cbegin(), m_configurations.cend(),
      [&id](const configuration &check) { return check.id == id; });
  if (found != m_configurations.cend()) {
    return *found;
  }
  return {};
}

configuration configuration_handler::get_or_throw(
    const wolf::types::uuid_array &id) const {
  const auto got = get(id);
  if (got) return got.value();
  std::stringstream error;
  error << "could not find configuration by id:" << id;
  throw std::runtime_error(error.str());
}

boost::optional<configuration> configuration_handler::get_by_room_id(
    const wolf::types::uuid_array &room_id) const {
  const configurations::const_iterator found = std::find_if(
      m_configurations.cbegin(), m_configurations.cend(),
      [&room_id](const configuration &check) { return check.room == room_id; });
  if (found == m_configurations.cend()) {
    return {};
  }
  return *found;
}

configuration configuration_handler::get_by_room_id_or_throw(
    const wolf::types::uuid_array &room_id) const {
  const auto result = get_by_room_id(room_id);
  if (result) return result.value();
  std::stringstream error;
  error << "could not fond configuration by room_id:" << room_id;
  throw std::runtime_error(error.str());
}

bool configuration_handler::remove_by_room_id(
    const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "removing configuration, room_id:" << room_id;
  const configurations::iterator found = std::find_if(
      m_configurations.begin(), m_configurations.end(),
      [&room_id](const configuration &check) { return check.room == room_id; });
  if (found == m_configurations.cend()) {
    return false;
  }
  const auto to_erase = found->id;
  remove(to_erase);
  return true;
}

void configuration_handler::load_all_from_database() {
  wolf::thread_checker::throw_if_not_service(
      "configuration_handler::load_all_from_database");
  // load out door sensor
  const wolf::sensor_id unset_sensor;
  m_outdoor_sensor.temperature =
      m_key_value_cache.get_outdoor_sensor().get_value_or(unset_sensor);
  m_outdoor_sensor.humidity =
      m_key_value_cache.get_outdoor_sensor_humidity().get_value_or(
          unset_sensor);
  LOGGING_LOG(m_logger, severity::verbose)
      << "got outdoor sensor from database,"
         " sensor:"
      << m_outdoor_sensor;
  LOGGING_LOG(m_logger, severity::verbose)
      << "getting all configurations from database";
  m_configurations = m_database.get_all();
  LOGGING_LOG(m_logger, severity::normal)
      << "got configurations from database, count:" << m_configurations.size();
  for (const auto &config : m_configurations) {
    signal_added(config);
  }
}
