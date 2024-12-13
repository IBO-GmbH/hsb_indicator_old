#ifndef WOLF_ROOM_CACHE_HPP
#define WOLF_ROOM_CACHE_HPP

#include "wolf/room.hpp"
#include "wolf/table_sync_helper.hpp"

namespace wolf {

class room_cache {
 public:
  using rooms = std::vector<room>;
  room_cache(wolf::database& database_);
  virtual ~room_cache() = default;

  virtual void add(const room& to_add);
  virtual void update(const room& to_update);
  virtual void remove(const wolf::types::uuid_array& to_remove);

  virtual rooms get_all();

 protected:
  logging::logger m_logger;
  using sync = table_sync_helper<room, std::string>;
  sync m_sync;
};
}  // namespace wolf

#endif  // WOLF_ROOM_CACHE_HPP
