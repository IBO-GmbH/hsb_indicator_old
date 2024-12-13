#ifndef WOLF_NETWORK_FOREIGN_SENSOR_RESPONSE_HPP
#define WOLF_NETWORK_FOREIGN_SENSOR_RESPONSE_HPP

#include "wolf/foreign_sensor.hpp"
#include "wolf/network/response/message.hpp"

namespace wolf::network::foreign_sensor {
class response {
 public:
  response(network::response::message& message_);

  using data = std::vector<wolf::foreign_sensor>;
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
}  // namespace wolf::network::foreign_sensor

#endif
