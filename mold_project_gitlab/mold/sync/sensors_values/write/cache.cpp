#include "cache.hpp"
#include "wolf/timestamp_helper.hpp"

using namespace mold::sync::sensors_values::write;

static constexpr auto table_name = "sync_values";

cache::cache(wolf::database &database)
    : m_logger{"sync::sensors_values::cache::write"},
      m_session(database.get_session()),
      m_block(false) {
  create_tables();
}

void cache::add(const wolf::sensor_value &value) {
  if (m_block) return;
  const std::string sensor = value.id.to_combined_id_base64();
  const auto timestamp =
      wolf::timestamp_helper().cast_to_unix_seconds(value.timestamp);
  const auto value_casted = static_cast<double>(value.value);
  m_session
      << "insert into " << table_name
      << " (sensor, value, timestamp) values(:sensor, :value, :timestamp)",
      soci::use(sensor), soci::use(value_casted), soci::use(timestamp);
}

std::vector<cache::entry> cache::get_some() {
  std::vector<entry> result;
  soci::rowset<soci::row> rows =
      (m_session.prepare << "select id, sensor, value, timestamp from "
                         << table_name << " limit 64");
  for (const auto &row : rows) {
    entry entry_;
    entry_.key_ = row.get<int>(0);
    entry_.value.id =
        wolf::sensor_id::from_combined_id_base64(row.get<std::string>(1));
    entry_.value.value = static_cast<float>(row.get<double>(2));
    entry_.value.timestamp = wolf::timestamp_helper().cast_seconds_to_chrono(
        static_cast<std::uint64_t>(row.get<int>(3)));
    result.push_back(entry_);
  }
  return result;
}

void cache::remove(const std::vector<cache::key> &keys) {
  // TODO this can be more optimized. eg prepared statement. has soci support
  // for delete "in" and vector?
  for (const auto item : keys) {
    m_session << "delete from " << table_name << " where id = :id",
        soci::use(item);
  }
}

void cache::clear_all_values_and_block_new_ones() {
  m_block = true;
  m_session << "delete from " << table_name;
}

void cache::remove_all_values_for_sensor_id(const wolf::sensor_id &id) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "delete all values for sensor: " << id;
  const std::string sensor = id.to_combined_id_base64();
  m_session << "delete from " << table_name << " where sensor = :sensor",
      soci::use(sensor);
}

void cache::create_tables() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "create_tables";
  m_session << "create table if not exists " << table_name
            << " (id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "sensor TEXT, "
               "value REAL, "
               "timestamp INTEGER"
               ")";
}
