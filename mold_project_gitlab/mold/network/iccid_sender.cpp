#include "iccid_sender.hpp"

using namespace mold::network;

iccid_sender::iccid_sender(
    wolf::network::bond_sender& sender,
    wolf::network::response::simple& response)
    : m_sender(sender), m_response(response) {}

void iccid_sender::send(
    const std::string_view iccid,
    const wolf::network::response::simple::callback& callback) {
  Clima::GsmInformation result;
  result.ICCID = iccid;
  const auto work_uuid =
      m_sender.send(result, Clima::Enums::MessageType::GsmInformation);
  m_response.register_work_uuid(work_uuid, callback);
}
