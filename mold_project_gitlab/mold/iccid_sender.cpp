#include "iccid_sender.hpp"
#include "wolf/uuid_generator.hpp"

using namespace logging;

mold::iccid_sender::iccid_sender(network::iccid_sender &sender,
                                 wolf::login_handler &login,
                                 gsm_information_reader &reader)
    : m_logger("iccid_sender"),
      m_sender(sender),
      m_last_iccid(optional_string()) {
  m_connection_logged_in = login.signal_logged_in.connect(
      [this](const bool logged_in) { send_iccid(logged_in); });
  m_connection_iccid = reader.signal_iccid.connect(
      [this](const auto &iccid) { handle_iccid_read(iccid); });
}

void mold::iccid_sender::send_iccid(const bool logged_in) {
  if (!logged_in || !m_last_iccid.has_value()) return;
  handle_send(m_last_iccid.value());
}

void mold::iccid_sender::handle_send(const std::string &iccid) {
  m_sender.send(iccid, [this](const auto &error) { handle_response(error); });
}

void mold::iccid_sender::handle_response(const optional_string &error) {
  if (!error) return;
  LOGGING_LOG(m_logger, severity::error)
      << "could not send ICCID! error: " << error.value();
}

void mold::iccid_sender::handle_iccid_read(const optional_string &iccid) {
  if (iccid != m_last_iccid) {
    m_last_iccid = iccid;
    if (iccid.has_value()) handle_send(iccid.value());
  }
}
