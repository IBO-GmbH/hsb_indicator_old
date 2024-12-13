#ifndef MOLD_SYNC_MOLD_SPROUT_CACHE_HPP
#define MOLD_SYNC_MOLD_SPROUT_CACHE_HPP

#include "mold/sprout_value.hpp"
#include "wolf/database.hpp"
#include "wolf/types.hpp"

namespace mold::sync::mold_sprout {

class cache {
 public:
  cache(wolf::database& database);

  [[nodiscard]] int add(const wolf::types::uuid_array& room_id,
                        const sprout_value& to_add);
  struct get_entry {
    int rowid;
    wolf::types::uuid_array room_id;
    sprout_value value;
  };
  std::vector<get_entry> get_all();
  void remove(const int rowid);

 private:
  void create_tables();

  logging::logger m_logger;
  soci::session& m_session;
};

}  // namespace mold::sync::mold_sprout

#endif
