#ifndef WOLF_NETWORK_LOGIN_RESPONSE_HPP
#define WOLF_NETWORK_LOGIN_RESPONSE_HPP

#include "log/logger.hpp"
#include "wolf/network/response/message.hpp"

namespace wolf::network::login {

class response {
 public:
  response(network::response::message& message_);

  struct data {
    bool resync;
  };
  // TODO C++20 or boost 1.70. use std::outcome<> or boost::outcome<>
  using callback = std::function<void(const std::optional<data>&)>;
  void register_work_uuid(const types::uuid_array& work_id,
                          const callback& to_call);

 private:
  void handle(const std::optional<Clima::Message>& message,
              const callback& to_call);

  logging::logger m_logger;
  network::response::message& m_message;
};
}  // namespace wolf::network::login

#endif
