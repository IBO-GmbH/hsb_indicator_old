#include "response.hpp"
#include "wolf/network/bond_deserializer.hpp"

using namespace wolf::network::login;

response::response(wolf::network::response::message &message_)
    : m_logger{"network::login::response"}, m_message(message_) {}

void response::register_work_uuid(const wolf::types::uuid_array &work_id,
                                  const response::callback &to_call) {
  m_message.register_work_uuid(work_id, [this, to_call](const auto &message) {
    handle(message, to_call);
  });
}

void response::handle(const std::optional<Clima::Message> &message,
                      const callback &to_call) {
  if (!message.has_value()) {
    LOGGING_LOG(m_logger, logging::severity::warning) << "Response timed out!";
    to_call({});
    return;
  }
  bond_deserializer deserializer;
  const auto response =
      deserializer.parse_message<Clima::Response>(message->Entry);
  if (response.Code != Clima::Enums::ResponseCode::Success) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "login got a non-success response. code:" << response.Code
        << ", message:" << response.Message;
    to_call({});
    return;
  }
  const auto response_login =
      deserializer.parse_message<Clima::LoginResponse>(message->Entry);
  to_call({{response_login.ResyncRequired}});
}
