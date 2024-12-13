#ifndef MOLD_SYNC_ROOM_DELETED_SENDER_HPP
#define MOLD_SYNC_ROOM_DELETED_SENDER_HPP

#include "log/logger.hpp"
#include "wolf/network/bond_sender.hpp"
#include "wolf/network/response/simple.hpp"

namespace mold::sync::room::deleted {

class sender {
 public:
  sender(wolf::network::bond_sender& sender,
         wolf::network::response::simple& response);

  void send(const wolf::types::uuid_array& room,
            const wolf::network::response::simple::callback& callback);

 private:
  logging::logger m_logger;
  wolf::network::bond_sender& m_sender;
  wolf::network::response::simple& m_reponse;
};

}  // namespace mold::sync::room::deleted

#endif
