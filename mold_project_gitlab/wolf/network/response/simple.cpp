#include "simple.hpp"
#include "wolf/network/bond_deserializer.hpp"

using namespace wolf::network::response;

simple::simple(message &message_) : m_message(message_) {}

void simple::register_work_uuid(const wolf::types::uuid_array &work_id,
                                const simple::callback &to_call) {
  m_message.register_work_uuid(work_id, [this, to_call](const auto &message_) {
    handle_message(to_call, message_);
  });
}

void simple::handle_message(const callback &to_call,
                            const std::optional<Clima::Message> &message) {
  if (!message.has_value()) {
    to_call("Response timed out!");
    return;
  }
  bond_deserializer deserializer;
  const auto response =
      deserializer.parse_message<Clima::Response>(message->Entry);
  if (response.Code == Clima::Enums::ResponseCode::Success) {
    to_call({});
    return;
  }
  to_call(response.Message);
}
