#ifndef MOLD_NETWORK_ICCID_SENDER_HPP
#define MOLD_NETWORK_ICCID_SENDER_HPP

#include "wolf/network/bond_sender.hpp"
#include "wolf/network/response/simple.hpp"

namespace mold::network {

class iccid_sender {
 public:
  iccid_sender(wolf::network::bond_sender& sender,
               wolf::network::response::simple& response);

  void send(const std::string_view iccid,
            const wolf::network::response::simple::callback& callback);

 private:
  wolf::network::bond_sender& m_sender;
  wolf::network::response::simple& m_response;
};

}  // namespace mold::network

#endif  // MOLD_NETWORK_ICCID_SENDER_HPP
