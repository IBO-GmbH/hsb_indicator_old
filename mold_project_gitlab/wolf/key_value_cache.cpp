#include "key_value_cache.hpp"

#include "wolf/base64.hpp"
#include "wolf/sensor_id.hpp"
#include "wolf/thread_checker.hpp"

#include <soci.h>

using namespace logging;
using namespace wolf;

key_value_cache::key_value_cache(wolf::database &database_)
    : base(database_), m_logger{"key_value_cache"} {
  create_tables();
}

void key_value_cache::set_language(const std::string &language) {
  wolf::thread_checker::throw_if_not_service("key_value_cache::set_language");
  return set("language", language);
}

std::string key_value_cache::get_language() {
  wolf::thread_checker::throw_if_not_service("key_value_cache::get_language");
  return get("language");
}

void key_value_cache::set_database_id(const types::uuid_array &id) {
  wolf::thread_checker::throw_if_not_service(
      "key_value_cache::set_database_id");
  const std::string result = base64::encode(id);
  set("configuration_id", result);
}

bool key_value_cache::get_database_id(types::uuid_array &fill) {
  wolf::thread_checker::throw_if_not_service(
      "key_value_cache::get_database_id");
  const std::string to_decode = get("configuration_id");
  if (to_decode.empty()) {
    return false;
  }
  const types::data encoded = base64::decode(to_decode);
  if (encoded.size() != fill.size()) {
    throw std::runtime_error(
        "key_value_cache::get_database_id: encoded size unequal fill size");
  }
  std::copy(encoded.cbegin(), encoded.cend(), fill.begin());
  return true;
}

void key_value_cache::set_outdoor_sensor(const sensor_id &id) {
  wolf::thread_checker::throw_if_not_service(
      "key_value_cache::set_outdoor_sensor");
  const std::string result = id.to_combined_id_base64();
  set("outdoor_sensor", result);
}

boost::optional<sensor_id> key_value_cache::get_outdoor_sensor() {
  wolf::thread_checker::throw_if_not_service(
      "key_value_cache::get_outdoor_sensor");
  const std::string to_decode = get("outdoor_sensor");
  if (to_decode.empty()) {
    return boost::optional<sensor_id>();
  }
  return sensor_id::from_combined_id_base64(to_decode);
}

void key_value_cache::set_outdoor_sensor_humidity(const sensor_id &id) {
  wolf::thread_checker::throw_if_not_service(
      "key_value_cache::set_outdoor_sensor_humidity");
  const std::string result = id.to_combined_id_base64();
  set("outdoor_sensor_humidity", result);
}

boost::optional<sensor_id> key_value_cache::get_outdoor_sensor_humidity() {
  wolf::thread_checker::throw_if_not_service(
      "key_value_cache::get_outdoor_sensor_humidity");
  const std::string to_decode = get("outdoor_sensor_humidity");
  if (to_decode.empty()) {
    return boost::optional<sensor_id>();
  }
  return sensor_id::from_combined_id_base64(to_decode);
}

void key_value_cache::set_outdoor_sensor_synced(const bool &synced) {
  wolf::thread_checker::throw_if_not_service(
      "key_value_cache::set_outdoor_sensor_synced");
  std::string result("0");
  if (synced) {
    result = "1";
  }
  set("outdoor_sensor_synced", result);
}

bool key_value_cache::get_outdoor_sensor_synced() {
  wolf::thread_checker::throw_if_not_service(
      "key_value_cache::get_outdoor_sensor_synced");
  const std::string to_decode = get("outdoor_sensor_synced");
  if (to_decode.empty()) {
    return false;
  }
  return to_decode == "1";
}

void key_value_cache::create_tables() {
  LOGGING_LOG(m_logger, severity::verbose) << "create_tables";
  m_session << "create table if not exists mold_options "
               "(key text primary key, "
               "value text not null)";
}

void key_value_cache::set(const std::string &key, const std::string &value) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set, key: " << key << ", value: " << value;
  m_session << "insert or replace into mold_options (key, value)"
               " values (:key, :value)",
      soci::use(key), soci::use(value);
  ensure_a_row_changed();
}

std::string key_value_cache::get(const std::string &key) {
  std::string result;
  m_session << "select value from mold_options where key = :key",
      soci::into(result), soci::use(key);
  return result;
}
