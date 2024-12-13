#ifndef MOLD_SYNC_ROOM_SETTINGS_WITH_SENSORS_SENDER_HPP
#define MOLD_SYNC_ROOM_SETTINGS_WITH_SENSORS_SENDER_HPP

#include "mold/building_type.hpp"
#include "mold/substrate_type.hpp"
#include "wolf/network/bond_sender.hpp"
#include "wolf/network/response/simple.hpp"
#include "wolf/sensor_profile.hpp"

namespace mold::sync::room::settings::with_sensors {

class sender {
 public:
  sender(wolf::network::bond_sender& sender,
         wolf::network::response::simple& response);
  struct information {
    const wolf::types::uuid_array room;
    const std::string name;
    const std::vector<wolf::sensor_profile> profiles;
    const float threshold_factor;
    const building_type building_type_;
    const substrate_type substrate_type_;
  };
  void send(const information& information_,
            const wolf::network::response::simple::callback& callback);

 private:
  logging::logger m_logger;
  wolf::network::bond_sender& m_sender;
  wolf::network::response::simple& m_reponse;
};

std::ostream& operator<<(std::ostream& out,
                         const sender::information& to_print);

}  // namespace mold::sync::room::settings::with_sensors

#endif
