#include "controller_cache.hpp"

#include <soci.h>
#include <boost/serialization/vector.hpp>
#include "boost_serialize_values_per_room.hpp"
#include "wolf/boost_serialize_helper.hpp"
#include "wolf/performance_profiler.hpp"

using namespace logging;

const std::string database_key("values");

wolf::controller::controller_cache::controller_cache(wolf::database &database_)
    : wolf::database_cache(database_), m_logger("controller_cache") {
  create_tables();
  load_all();
}

bool wolf::controller::controller_cache::add(
    const wolf::controller::values_per_room &to_add) {
  LOGGING_LOG(m_logger, severity::verbose) << "add, to_add: " << to_add;
  auto found = find(to_add.room_id);
  if (found != m_values.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not add values_list, because found != m_values.end(), id: "
        << to_add.room_id;
    return false;
  }
  m_values.push_back(to_add);
  save_all();
  return true;
}

bool wolf::controller::controller_cache::update(
    const wolf::controller::values_per_room &to_update) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "update, to_update: " << to_update;
  auto found = find(to_update.room_id);
  if (found == m_values.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not update values_list, because found == m_values.end(), id: "
        << to_update.room_id;
    return false;
  }
  if (*found == to_update) {
    LOGGING_LOG(m_logger, severity::verbose) << "update, nothing changed";
    return true;
  }
  *found = to_update;
  save_all();
  return true;
}

bool wolf::controller::controller_cache::remove(
    const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, severity::verbose) << "remove, room_id: " << room_id;
  auto found = find(room_id);
  if (found == m_values.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not remove values_list, because found == m_values.end(), id: "
        << room_id;
    return false;
  }
  m_values.erase(found);
  save_all();
  return true;
}

wolf::controller::controller_cache::values_list
wolf::controller::controller_cache::get_all() {
  return m_values;
}

void wolf::controller::controller_cache::create_tables() {
  m_session << "create table if not exists controller_cache "
               "(key text primary key, "
               "value text not null)";
}

wolf::controller::controller_cache::values_list::iterator
wolf::controller::controller_cache::find(
    const wolf::types::uuid_array &room_id) {
  return std::find_if(m_values.begin(), m_values.end(),
                      [&room_id](const values_per_room &values) {
                        return values.room_id == room_id;
                      });
}

void wolf::controller::controller_cache::write(const std::string &key,
                                               const std::string &value) {
  m_session << "insert or replace into controller_cache (key, value)"
            << " values (:key, :value)",
      soci::use(key), soci::use(value);
  ensure_a_row_changed();
}

std::string wolf::controller::controller_cache::get(const std::string &key) {
  std::string result;
  m_session << "select value from controller_cache where key = :key",
      soci::into(result), soci::use(key);
  return result;
}

void wolf::controller::controller_cache::save_all() {
  const std::string value = wolf::serialize::encode_to_base64(m_values);
  write(database_key, value);
}

void wolf::controller::controller_cache::load_all() {
  LOGGING_LOG(m_logger, severity::verbose) << "load_all()";
  const auto saved = get(database_key);
  if (saved.empty()) {
    LOGGING_LOG(m_logger, severity::normal) << "nothing to load";
    return;
  }
  wolf::serialize::decode_from_base64(saved, m_values);
}
