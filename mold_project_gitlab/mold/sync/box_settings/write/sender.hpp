#ifndef MOLD_SYNC_BOX_SETTINGS_WRITE_SENDER_HPP
#define MOLD_SYNC_BOX_SETTINGS_WRITE_SENDER_HPP

#include "mold/sync/box_settings/entity.hpp"
#include "wolf/network/bond_sender.hpp"
#include "wolf/network/response/simple.hpp"
#include "wolf/profile_handler.hpp"

namespace mold::sync::box_settings::write {

class sender {
 public:
  sender(wolf::network::bond_sender& sender,
         wolf::network::response::simple& response,
         wolf::profile_handler& profile_handler, const bool send_rssi);
  void send(const entity& to_send,
            const wolf::network::response::simple::callback& callback);

 private:
  using sensor_map =
      std::map< ::Clima::Enums::SensorType, ::Clima::SensorProfile>;

  void add_sensor_to_map(sensor_map& to_add_to, const wolf::sensor_id& to_add);

 private:
  logging::logger m_logger;
  wolf::network::bond_sender& m_sender;
  wolf::network::response::simple& m_response;
  wolf::profile_handler& m_profile_handler;
  const bool m_send_rssi;
};

}  // namespace mold::sync::box_settings::write
#endif
