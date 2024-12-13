#ifndef WOLF_DATABASE_CACHE_HPP
#define WOLF_DATABASE_CACHE_HPP

#include <cstdint>
#include "wolf/database.hpp"

namespace soci {
class session;
}

namespace wolf {

// TODO rename to database_helper and DON'T derive it
class database_cache {
 public:
  database_cache(wolf::database& database_);
  database_cache(soci::session& session);
  database_cache(const database_cache&) = delete;

  bool ensure_a_row_changed();
  std::size_t get_rows_changed();
  std::int64_t get_last_rowid();

 protected:
  soci::session& m_session;
};
}  // namespace wolf

#endif  // WOLF_DATABASE_CACHE_HPP
