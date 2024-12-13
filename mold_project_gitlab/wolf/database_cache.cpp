#include "database_cache.hpp"

#include "database.hpp"

using namespace wolf;

database_cache::database_cache(wolf::database& database_)
    : database_cache(database_.get_session()) {}

database_cache::database_cache(soci::session& session) : m_session(session) {}

bool database_cache::ensure_a_row_changed() {
  const std::size_t change{get_rows_changed()};
  if (change < 1)
    throw std::runtime_error(
        "database_cache::ensure_a_row_changed: no row changed");
  return change >= 1;
}

std::size_t database_cache::get_rows_changed() {
  std::size_t change;
  m_session << "select changes()", soci::into(change);
  return change;
}

int64_t database_cache::get_last_rowid() {
  int64_t result{};
  m_session << "select last_insert_rowid()", soci::into(result);
  return result;
}
