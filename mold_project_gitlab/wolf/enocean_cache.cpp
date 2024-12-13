#include "enocean_cache.hpp"
#include "wolf/base64.hpp"

using namespace wolf;

static constexpr auto table_name = "enocean_eep";

enocean_cache::enocean_cache(database& database_)
    : m_logger{"enocean_cache"}, m_session(database_.get_session()) {
  create_tables();
}

void enocean_cache::add(const enocean_cache::entry& entry_) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "add, id:" << std::hex << entry_.id << ", eep:" << entry_.eep
      << std::dec;
  const auto eep_casted = base64::encode(entry_.eep);
  m_session << "insert or replace into " << table_name
            << " (id, eep)"
               " values (:id, :eep)",
      soci::use(entry_.id), soci::use(eep_casted);
}

void enocean_cache::remove_devices(const std::vector<types::id_esp3>& ids) {
  for (const auto& id : ids) remove(id);
}

void enocean_cache::remove(const types::id_esp3 id) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "remove, id:" << std::hex << id;
  m_session << "delete from " << table_name << " where id = :id", soci::use(id);
}

std::vector<enocean_cache::entry> enocean_cache::get_all() {
  std::vector<entry> result;
  soci::rowset<soci::row> result_ =
      (m_session.prepare << "select id, eep from " << table_name);
  for (const auto& row : result_) {
    const auto id = static_cast<types::id_esp3>(row.get<int>(0));
    const auto to_copy = wolf::base64::decode(row.get<std::string>(1));
    BOOST_ASSERT(to_copy.size() == 3);
    types::eep eep;
    std::copy(to_copy.cbegin(), to_copy.cend(), eep.begin());
    const entry result_entry{id, eep};
    result.push_back(result_entry);
  }
  return result;
}

void enocean_cache::create_tables() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "create_tables";
  m_session << "create table if not exists " << table_name
            << " (id INTEGER PRIMARY KEY, "
               "eep TEXT)";
}
