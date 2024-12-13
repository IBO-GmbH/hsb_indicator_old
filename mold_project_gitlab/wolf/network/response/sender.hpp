#ifndef WOLF_NETWORK_RESPONSE_SENDER_HPP
#define WOLF_NETWORK_RESPONSE_SENDER_HPP

#include "wolf/network/bond_sender.hpp"

namespace wolf::network::response {
class sender {
 public:
  sender(bond_sender& sender);

  void send_success(const types::uuid_array& confirmation);
  void send_failure(const types::uuid_array& confirmation,
                    const std::string_view description);

 private:
  logging::logger m_logger;
  bond_sender& m_sender;
};
}  // namespace wolf::network::response

#endif
