#include "cache.hpp"
#include "wolf/uuid_helper.hpp"

using namespace mold::sync::room;

static constexpr auto table_name = "sync_room_settings";

struct cache::entry {
  std::string room;
  int version;
  int dirty;
  int deleted;
  int added;
};

namespace soci {
template <>
struct type_conversion<cache::entry> {
  typedef values base_type;
  static void from_base(values const &values_, indicator /*ind*/,
                        cache::entry &result) {
    result.room = values_.get<std::string>("room");
    result.version = values_.get<int>("version");
    result.dirty = values_.get<int>("dirty");
    result.deleted = values_.get<int>("deleted");
    result.added = values_.get<int>("added");
  }
  static void to_base(const cache::entry &entry_, values &values_,
                      indicator &indicator_) {
    values_.set("room", entry_.room);
    values_.set("version", entry_.version);
    values_.set("dirty", entry_.dirty);
    values_.set("deleted", entry_.deleted);
    values_.set("added", entry_.added);
    indicator_ = i_ok;
  }
};
}  // namespace soci

cache::cache(wolf::database &database)
    : m_logger{"sync::room::cache"}, m_session(database.get_session()) {
  create_tables();
}

void cache::mark_dirty(const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "mark_dirty";
  wolf::uuid_helper uuid_helper;
  const auto room_id_casted = uuid_helper.array_to_string(room_id);
  const auto already_available = get(room_id_casted);
  if (!already_available) {
    insert(room_id_casted);
    return;
  }
  entry update_ = already_available.value();
  if (update_.deleted == 1) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "tried to mark a deleted entry as dirty. Won't do anything";
    throw std::runtime_error("tried to mark a deleted entry as dirty");
  }
  update_.dirty = 1;
  ++update_.version;
  update(update_);
}

void cache::mark_deleted(const wolf::types::uuid_array &room_id) {
  wolf::uuid_helper uuid_helper;
  const auto room_id_casted = uuid_helper.array_to_string(room_id);
  m_session << "update " << table_name
            << " set deleted = 1"
               " where room = :room",
      soci::use(room_id_casted);
}

void cache::raise_version(const wolf::types::uuid_array &room_id) {
  const auto room_id_casted = wolf::uuid_helper().array_to_string(room_id);
  const auto entry = get(room_id_casted);
  if (!entry) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "tried to raise the version of a deleted entry. Won't do anything";
    return;
  }
  auto entry_value = entry.value();
  ++entry_value.version;
  update(entry_value);
}

static cache::dirty cast_entry_to_dirty(const cache::entry &entry) {
  const auto room = wolf::uuid_helper().string_to_array(entry.room);
  cache::dirty result{room, entry.version};
  return result;
}

std::vector<cache::dirty> cache::get_dirty_but_not_deleted() {
  soci::rowset<entry> result_sql =
      (m_session.prepare << "select * from " << table_name
                         << " where deleted == 0 and dirty == 1");
  std::vector<cache::dirty> result;
  std::transform(result_sql.begin(), result_sql.end(),
                 std::back_inserter(result), cast_entry_to_dirty);
  return result;
}

static wolf::types::uuid_array cast_entry_to_id(const cache::entry &entry) {
  const auto result = wolf::uuid_helper().string_to_array(entry.room);
  return result;
}

std::vector<wolf::types::uuid_array> cache::get_deleted() {
  handle_delete_of_not_synced_rooms();
  soci::rowset<entry> result_sql =
      (m_session.prepare << "select * from " << table_name
                         << " where deleted == 1");
  std::vector<wolf::types::uuid_array> result;
  std::transform(result_sql.begin(), result_sql.end(),
                 std::back_inserter(result), cast_entry_to_id);
  return result;
}

void cache::synchronized(const wolf::types::uuid_array &room,
                         const int version) {
  wolf::uuid_helper uuid_helper;
  const auto room_id_casted = uuid_helper.array_to_string(room);
  m_session << "update " << table_name
            << " set dirty = 0, added = 0"
               " where version = :version and room = :room",
      soci::use(version), soci::use(room_id_casted);
}

void cache::deleted(const wolf::types::uuid_array &room) {
  wolf::uuid_helper uuid_helper;
  const auto room_id_casted = uuid_helper.array_to_string(room);
  m_session << "delete from " << table_name << " where room = :room",
      soci::use(room_id_casted);
}

bool cache::is_marked_or_actually_deleted(const wolf::types::uuid_array &room) {
  wolf::uuid_helper uuid_helper;
  const auto room_id_casted = uuid_helper.array_to_string(room);
  int result{};
  m_session << "select deleted from " << table_name << " where room = :search",
      soci::into(result), soci::use(room_id_casted, "search");
  if (!m_session.got_data()) return false;
  return result != 0;
}

void cache::create_tables() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "create_tables";
  m_session << "create table if not exists " << table_name
            << " (room TEXT primary key, "
               "version INTEGER, "
               "dirty INTEGER, "
               "deleted INTEGER"
               ")";
  add_column_if_not_exists();
}

std::optional<cache::entry> cache::get(const std::string &room_id) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "get, room_id:" << room_id;
  entry result;
  m_session << "select room, version, dirty, deleted, added from " << table_name
            << " where room = :search",
      soci::into(result), soci::use(room_id, "search");
  if (!m_session.got_data()) return {};
  return {result};
}

void cache::update(const cache::entry &write) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "update, room_id:" << write.room;
  m_session << "update " << table_name
            << " set version = :version, dirty = :dirty, deleted "
               "= :deleted, added = :added where room = :room",
      soci::use(write);
}

void cache::insert(const std::string &room_id) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "insert, room_id:" << room_id;
  entry write;
  write.room = room_id;
  write.dirty = 1;
  write.version = 0;
  write.deleted = 0;
  write.added = 1;
  m_session
      << "insert into " << table_name
      << " (room, version, dirty, deleted, added) values(:room, :version, "
         ":dirty, :deleted, :added)",
      soci::use(write);
}

void cache::handle_delete_of_not_synced_rooms() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "delete all rooms, which were added and deleted but never synced";
  m_session << "delete from " << table_name
            << " where deleted == 1 and added == 1";
}

void cache::add_column_if_not_exists() {
  try {
    soci::rowset<soci::row> rows =
        (m_session.prepare << "select added from " << table_name);
  } catch (std::runtime_error &) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "column 'added' delivered no rows, so it does not exist --> add it";
    m_session << "alter table " << table_name << " add added INTEGER";
    m_session << "update " << table_name << " set added = 0";
  }
}
