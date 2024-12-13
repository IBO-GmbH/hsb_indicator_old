#ifndef WOLF_ENOCEAN_CACHE_HPP
#define WOLF_ENOCEAN_CACHE_HPP

#include "wolf/database.hpp"
#include "wolf/types.hpp"

namespace wolf {

class enocean_cache {
 public:
  enocean_cache(database& database_);

  struct entry {
    types::id_esp3 id;
    const types::eep eep;
  };
  void add(const entry& entry_);
  void remove_devices(const std::vector<types::id_esp3>& ids);
  std::vector<entry> get_all();

 private:
  void create_tables();
  void remove(const types::id_esp3 id);

  logging::logger m_logger;
  soci::session& m_session;
};
}  // namespace wolf

#endif
