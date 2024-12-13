#ifndef WOLF_NETWORK_FOREIGN_SENSOR_REQUESTER_HPP
#define WOLF_NETWORK_FOREIGN_SENSOR_REQUESTER_HPP

#include "wolf/network/bond_sender.hpp"
#include "wolf/network/foreign_sensor/response.hpp"
#include "wolf/sensor_value_type.hpp"

namespace wolf::network::foreign_sensor {

class requester {
 public:
  requester(bond_sender& sender, response& response_);

  void request(const response::callback& callback);

 private:
  logging::logger m_logger;
  bond_sender& m_sender;
  response& m_response;
};

}  // namespace wolf::network::foreign_sensor

#endif
