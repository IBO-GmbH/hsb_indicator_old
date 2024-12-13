#include "sender.hpp"
#include "wolf/uuid_helper.hpp"

using namespace wolf::network::response;
using namespace wolf;

sender::sender(bond_sender& sender)
    : m_logger{"network::response::sender"}, m_sender(sender) {}

static void send(wolf::network::bond_sender& sender_,
                 const types::uuid_array& confirmation,
                 Clima::Response& to_send) {
  to_send.ConfirmationUUID = uuid_helper().array_to_vector(confirmation);
  sender_.send_no_response_wanted(to_send, Clima::Enums::MessageType::Response);
}

void sender::send_success(const types::uuid_array& confirmation) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "send_success:" << confirmation;
  Clima::Response result;
  result.Code = Clima::Enums::ResponseCode::Success;
  send(m_sender, confirmation, result);
}

void sender::send_failure(const types::uuid_array& confirmation,
                          const std::string_view description) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "send_failure:" << confirmation;
  Clima::Response result;
  result.Message = description;
  result.Code = Clima::Enums::ResponseCode::Unknown;
  send(m_sender, confirmation, result);
}
