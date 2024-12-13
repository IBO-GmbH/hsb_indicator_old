#include "cache.hpp"
#include "wolf/database_cache.hpp"
#include "wolf/timestamp_helper.hpp"
#include "wolf/uuid_helper.hpp"

using namespace mold::sync::mold_sprout;

static constexpr auto table_name = "sync_mold_sprout";

cache::cache(wolf::database &database)
    : m_logger{"sync::mold_sprout::cache"}, m_session(database.get_session()) {
  create_tables();
}

int cache::add(const wolf::types::uuid_array &room_id,
               const mold::sprout_value &to_add) {
  const std::string room = wolf::uuid_helper().array_to_string(room_id);
  wolf::timestamp_helper timestamp_helper;
  const auto timestamp_start =
      timestamp_helper.cast_to_unix_seconds(to_add.timestamp_start);
  const auto timestamp_change =
      timestamp_helper.cast_to_unix_seconds(to_add.timestamp_change);
  const auto value_casted = static_cast<int>(to_add.sprout_type_);
  m_session << "insert into " << table_name
            << " (room_id, value, timestamp_start, timestamp_change) "
               "values(:room_id, :value, :timestamp_start, :timestamp_change)",
      soci::use(room), soci::use(value_casted), soci::use(timestamp_start),
      soci::use(timestamp_change);
  wolf::database_cache helper(m_session);
  return static_cast<int>(helper.get_last_rowid());
}

static std::chrono::system_clock::time_point cast_int_to_chrono(
    const int to_cast) {
  const wolf::timestamp_helper timestamp_helper;
  return timestamp_helper.cast_seconds_to_chrono(
      static_cast<std::uint64_t>(to_cast));
}

std::vector<cache::get_entry> cache::get_all() {
  std::vector<get_entry> result;
  soci::rowset<soci::row> rows =
      (m_session.prepare
       << "select id, room_id, value, timestamp_start, timestamp_change from "
       << table_name);
  wolf::uuid_helper uuid_helper;
  for (const auto &row : rows) {
    get_entry entry;
    entry.rowid = row.get<int>(0);
    entry.room_id = uuid_helper.string_to_array(row.get<std::string>(1));
    entry.value.sprout_type_ = static_cast<sprout_type>(row.get<int>(2));
    entry.value.timestamp_start = cast_int_to_chrono(row.get<int>(3));
    entry.value.timestamp_change = cast_int_to_chrono(row.get<int>(4));
    result.push_back(entry);
  }
  return result;
}

void cache::remove(const int rowid) {
  m_session << "delete from " << table_name << " where id = :id",
      soci::use(rowid);
}

void cache::create_tables() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "create_tables";
  m_session << "create table if not exists " << table_name
            << " (id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "room_id TEXT, "
               "value INTEGER, "
               "timestamp_start INTEGER, "
               "timestamp_change INTEGER"
               ")";
}
