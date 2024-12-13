#include "sender.hpp"

using namespace wolf::network::login;

sender::sender(bond_sender &destination, response &response)
    : m_logger{"network::login::sender"},
      m_destination(destination),
      m_response(response) {}

void sender::send(const sender::data &to_send,
                  const sender::on_complete &handler) {
  Clima::Login login;
  login.Version = to_send.version;
  login.MacAddress = to_send.id;
  login.Secret = to_send.secret;
  login.Capabilities = to_send.capabilities;
  std::copy(to_send.database_id.cbegin(), to_send.database_id.cend(),
            std::back_inserter(login.DeviceDatabaseUUID));
  const auto work_uuid =
      m_destination.send(login, Clima::Enums::MessageType::Login);
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "sent message, version:" << login.Version
      << ", MacAddress:" << login.MacAddress << ", Secret:" << login.Secret
      << ", Capabilities size:" << login.Capabilities.size();
  m_response.register_work_uuid(work_uuid, handler);
}
