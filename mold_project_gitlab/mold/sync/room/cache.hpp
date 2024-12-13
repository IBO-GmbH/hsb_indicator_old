#ifndef MOLD_SYNC_ROOM_CACHE_HPP
#define MOLD_SYNC_ROOM_CACHE_HPP

#include "wolf/database.hpp"
#include "wolf/types.hpp"

namespace mold::sync::room {

class cache {
 public:
  struct entry;

  cache(wolf::database& database);
  void mark_dirty(const wolf::types::uuid_array& room_id);
  void mark_deleted(const wolf::types::uuid_array& room_id);
  void raise_version(const wolf::types::uuid_array& room_id);
  struct dirty {
    wolf::types::uuid_array room_id;
    int version;
  };
  std::vector<dirty> get_dirty_but_not_deleted();
  std::vector<wolf::types::uuid_array> get_deleted();
  void synchronized(const wolf::types::uuid_array& room, const int version);
  void deleted(const wolf::types::uuid_array& room);
  bool is_marked_or_actually_deleted(const wolf::types::uuid_array& room);

 private:
  void create_tables();
  std::optional<entry> get(const std::string& room_id);
  void update(const entry& write);
  void insert(const std::string& room_id);
  void handle_delete_of_not_synced_rooms();
  void add_column_if_not_exists();

 private:
  logging::logger m_logger;
  soci::session& m_session;
};

}  // namespace mold::sync::room

#endif
