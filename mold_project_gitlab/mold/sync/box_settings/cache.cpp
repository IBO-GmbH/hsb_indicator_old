#include "cache.hpp"

using namespace mold::sync::box_settings;

static constexpr auto table_name = "sync_box_settings";

cache::cache(wolf::database &database)
    : m_logger{"sync::box_settings::cache"}, m_session(database.get_session()) {
  create_tables();
}

void cache::mark_dirty() {
  dirty change = get();
  ++change.version;
  change.is_dirty = true;
  set(change);
}

void cache::synchronized(const int version) {
  m_session << "update " << table_name
            << " set dirty = 0"
               " where version = :version",
      soci::use(version);
}

void cache::raise_version() {
  dirty change = get();
  ++change.version;
  set(change);
}

cache::dirty cache::get() {
  dirty result;
  int is_dirty{};
  m_session << "select version, dirty from " << table_name,
      soci::into(result.version), soci::into(is_dirty);
  result.is_dirty = is_dirty != 0;
  return result;
}

void cache::create_tables() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "create_tables";
  m_session << "create table if not exists " << table_name
            << " (box TEXT primary key, "
               "version INTEGER, "
               "dirty INTEGER"
               ")";
}

void cache::set(const dirty to_set) {
  m_session << "delete from " << table_name;
  const int is_dirty = to_set.is_dirty ? 1 : 0;
  m_session << "insert into " << table_name
            << " (box, version, dirty) VALUES ('box', :version, :dirty)",
      soci::use(to_set.version), soci::use(is_dirty);
}
