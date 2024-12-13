#ifndef WOLF_NETWORK_RESPONSE_SEND_HELPER_HPP
#define WOLF_NETWORK_RESPONSE_SEND_HELPER_HPP

#include "log/logger.hpp"
#include "wolf/network/response/sender.hpp"

namespace wolf::network::response {

class send_helper {
 public:
  send_helper(sender& sender_, const types::uuid_array& work_uuid);
  void success();
  void failure(const std::string& description);

 private:
  logging::logger m_logger;
  sender& m_sender;
  types::uuid_array m_work_uuid;
};

}  // namespace wolf::network::response

#endif
