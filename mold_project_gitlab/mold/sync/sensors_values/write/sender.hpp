#ifndef MOLD_SYNC_SENSORS_VALUES_WRITE_SENDER_HPP
#define MOLD_SYNC_SENSORS_VALUES_WRITE_SENDER_HPP

#include "wolf/network/bond_sender.hpp"
#include "wolf/network/response/simple.hpp"
#include "wolf/sensor_value.hpp"

namespace mold::sync::sensors_values::write {

class sender {
 public:
  sender(wolf::network::bond_sender& sender,
         wolf::network::response::simple& response);
  void send(const std::vector<wolf::sensor_value>& to_send,
            const wolf::network::response::simple::callback& callback);

 private:
  Clima::SensorsValues cast_to_network(std::vector<wolf::sensor_value> to_send);

  logging::logger m_logger;
  wolf::network::bond_sender& m_sender;
  wolf::network::response::simple& m_reponse;
};
}  // namespace mold::sync::sensors_values::write

#endif
