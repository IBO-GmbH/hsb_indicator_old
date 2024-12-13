#ifndef MOLD_ICCID_SENDER_HPP
#define MOLD_ICCID_SENDER_HPP

#include "mold/gsm_information_reader.hpp"
#include "mold/network/iccid_sender.hpp"
#include "wolf/login_handler.hpp"

namespace mold {

class iccid_sender {
 public:
  using optional_string = std::optional<std::string>;

  iccid_sender(network::iccid_sender &sender, wolf::login_handler &login,
               gsm_information_reader &reader);
  void send_iccid(const bool logged_in);

 private:
  void handle_send(const std::string &iccid);
  void handle_response(const optional_string &error);
  void handle_iccid_read(const optional_string &iccid);

 private:
  logging::logger m_logger;
  network::iccid_sender &m_sender;

  boost::signals2::scoped_connection m_connection_logged_in;
  boost::signals2::scoped_connection m_connection_iccid;

  optional_string m_last_iccid;
};
}  // namespace mold

#endif  // MOLD_ICCID_SENDER_HPP
