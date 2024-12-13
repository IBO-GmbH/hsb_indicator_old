#include "sender.hpp"
#include "wolf/uuid_helper.hpp"

using namespace mold::sync::room::deleted;

sender::sender(wolf::network::bond_sender &sender,
               wolf::network::response::simple &response)
    : m_logger{"sync::room::deleted::sender"},
      m_sender(sender),
      m_reponse(response) {}

void sender::send(const wolf::types::uuid_array &room,
                  const wolf::network::response::simple::callback &callback) {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "send, room:" << room;
  Clima::RoomDelete result;
  result.RoomUUID = wolf::uuid_helper().array_to_vector(room);
  const auto work_uuid =
      m_sender.send(result, Clima::Enums::MessageType::RoomDelete);
  m_reponse.register_work_uuid(work_uuid, callback);
}
