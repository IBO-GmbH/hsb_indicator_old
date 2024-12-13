#ifndef MOLD_SYNC_ROOM_DELETED_HANDLER_HPP
#define MOLD_SYNC_ROOM_DELETED_HANDLER_HPP

#include "mold/sync/room/cache.hpp"
#include "mold/sync/room/deleted/sender.hpp"
#include "wolf/login_handler.hpp"
#include "wolf/room_handler.hpp"

namespace mold::sync::room::deleted {

class handler {
 public:
  handler(cache& cache_, sender& sender_, wolf::room_handler& rooms,
          wolf::login_handler& login);

  void resync();

 private:
  void connect_signals();
  void mark_deleted(const wolf::types::uuid_array& room_id);
  void sync(const wolf::types::uuid_array& room_id);
  void handle_room_removed(const wolf::types::uuid_array& room_id);
  void handle_response(const std::optional<std::string>& error,
                       const wolf::types::uuid_array& room);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  cache& m_cache;
  sender& m_sender;
  wolf::room_handler& m_rooms;
  wolf::login_handler& m_login;
};

}  // namespace mold::sync::room::deleted

#endif
