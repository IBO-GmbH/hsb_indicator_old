#include "sender.hpp"
#include "mold/network/sprout_type_helper.hpp"
#include "wolf/timestamp_helper.hpp"
#include "wolf/uuid_helper.hpp"

using namespace logging;

mold::sync::mold_sprout::sender_implementation::sender_implementation(
    wolf::network::bond_sender &sender,
    wolf::network::response::simple &response)
    : m_logger{"sync::mold_sprout::sender"},
      m_sender(sender),
      m_reponse(response) {}

void mold::sync::mold_sprout::sender_implementation::send(
    const mold_sprout_value &to_send,
    const wolf::network::response::simple::callback &callback) {
  const Clima::MoldSprout result = cast_to_network(to_send);
  const auto work_uuid =
      m_sender.send(result, Clima::Enums::MessageType::MoldSproutValue);
  m_reponse.register_work_uuid(work_uuid, callback);
}

Clima::MoldSprout
mold::sync::mold_sprout::sender_implementation::cast_to_network(
    mold_sprout_value to_send) {
  Clima::MoldSprout result;
  result.RoomUUID = wolf::uuid_helper().array_to_vector(to_send.room_id);
  result.SproutType =
      network::sprout_type_helper().cast_to_network(to_send.value.sprout_type_);
  wolf::timestamp_helper timestamp_helper_;
  result.TimestampStart =
      timestamp_helper_.cast_to_unix_seconds(to_send.value.timestamp_start);
  result.TimestampChange =
      timestamp_helper_.cast_to_unix_seconds(to_send.value.timestamp_change);
  return result;
}
