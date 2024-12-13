#ifndef WOLF_NETWORK_RESPONSE_SIMPLE_HPP
#define WOLF_NETWORK_RESPONSE_SIMPLE_HPP

#include "wolf/network/response/message.hpp"

namespace wolf::network::response {
class simple {
 public:
  simple(message& message_);
  virtual ~simple() = default;

  // TODO C++20/"boost 1.70" refactor to outcome
  using callback = std::function<void(const std::optional<std::string>&)>;
  virtual void register_work_uuid(const types::uuid_array& work_id,
                                  const callback& to_call);

 private:
  void handle_message(const callback& to_call,
                      const std::optional<Clima::Message>&);

  message& m_message;
};
}  // namespace wolf::network::response

#endif
