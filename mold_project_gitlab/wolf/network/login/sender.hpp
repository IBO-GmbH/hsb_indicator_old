#ifndef WOLF_NETWORK_LOGIN_SENDER_HPP
#define WOLF_NETWORK_LOGIN_SENDER_HPP

#include "log/logger.hpp"
#include "wolf/network/bond_sender.hpp"
#include "wolf/network/login/response.hpp"

namespace wolf::network::login {

class sender {
 public:
  sender(bond_sender& destination, response& response);
  using on_complete = response::callback;
  struct data {
    const types::id id;
    const std::string version;
    const types::uuid_array database_id;
    const types::data secret;
    const std::map<std::string, bool> capabilities;
  };
  void send(const data& to_send, const on_complete& handler);

 private:
  logging::logger m_logger;
  bond_sender& m_destination;
  response& m_response;
};

}  // namespace wolf::network::login
#endif
