#ifndef WOLF_NETWORK_BOND_SENDER_HPP
#define WOLF_NETWORK_BOND_SENDER_HPP

#include <bond/core/bond.h>
#include "mold_protocol_reflection.h"
#include "wolf/types.hpp"
#include "wolf/uuid_generator.hpp"
#include "wolf/websocket_sync.hpp"

namespace wolf::network {

class bond_sender {
 public:
  using package = types::data;

  bond_sender(wolf::websocket_sync& destination);

  template <typename message_type>
  [[nodiscard]] types::uuid_array send(const message_type& to_send,
                                       const Clima::Enums::MessageType& type);
  template <typename message_type>
  void send_no_response_wanted(const message_type& to_send,
                               const Clima::Enums::MessageType& type);

 private:
  template <typename message_type>
  static package generate_message(const message_type& to_send,
                                  const Clima::Enums::MessageType& type,
                                  const types::data& work_uuid);
  template <typename message_type>
  static bond::bonded<message_type> serialize_polymorphic_bond(
      const message_type& obj);

  wolf::websocket_sync& m_destination;
};

template <typename message_type>
void bond_sender::send_no_response_wanted(
    const message_type& to_send, const Clima::Enums::MessageType& type) {
  const types::data work_uuid;
  const types::data result = generate_message(to_send, type, work_uuid);
  m_destination.send(result);
}

template <typename message_type>
[[nodiscard]] types::uuid_array bond_sender::send(
    const message_type& to_send, const Clima::Enums::MessageType& type) {
  const auto work_uuid = uuid_generator::generate_as_array();
  const types::data work_uuid_casted(work_uuid.cbegin(), work_uuid.cend());
  const types::data result = generate_message(to_send, type, work_uuid_casted);
  m_destination.send(result);
  return work_uuid;
}

template <typename message_type>
bond_sender::package bond_sender::generate_message(
    const message_type& to_send, const Clima::Enums::MessageType& type,
    const types::data& work_uuid) {
  auto to_send_copy = to_send;
  to_send_copy.WorkUUID = work_uuid;
  Clima::Message holder;
  holder.Type = type;
  holder.Entry = serialize_polymorphic_bond(to_send_copy);

  bond::OutputBuffer output;
  bond::CompactBinaryWriter<bond::OutputBuffer> writer(output);
  bond::Serialize(holder, writer);
  const bond::blob to_send_buffer = output.GetBuffer();
  package result;
  std::copy_n(to_send_buffer.content(), to_send_buffer.length(),
              std::back_inserter(result));

  return result;
}

template <typename message_type>
bond::bonded<message_type> bond_sender::serialize_polymorphic_bond(
    const message_type& obj) {
  bond::OutputBuffer buffer;
  bond::CompactBinaryWriter<bond::OutputBuffer> writer(buffer);
  bond::Serialize(obj, writer);
  bond::CompactBinaryReader<bond::InputBuffer> reader(buffer.GetBuffer());

  return bond::bonded<message_type>(reader);
}

}  // namespace wolf::network
#endif
