#ifndef MOLD_CONTROLLER_HANDLER_HPP
#define MOLD_CONTROLLER_HANDLER_HPP

#include "mold/configuration_handler.hpp"
#include "mold/configuration_values_handler.hpp"
#include "wolf/actor/handler.hpp"
#include "wolf/controller/controller_handler.hpp"
#include "wolf/room_handler.hpp"

namespace mold {

class controller_handler {
 public:
  controller_handler(wolf::controller::controller_handler& controller_handler,
                     wolf::actor::handler& actor_handler,
                     wolf::room_handler& room_handler,
                     wolf::sensor_value_handler& value_handler,
                     configuration_handler& configuration_handler_,
                     wolf::sensor_value_offset_handler& offset_handler,
                     configuration_values_handler& config_values_handler,
                     wolf::profile_handler& profile_handler);

 private:
  void set_up_controller_signals();
  void set_up_room_signals();
  void set_up_value_signals();
  void set_up_configuration_signals();
  std::vector<wolf::room> get_rooms_with_sensor(
      const std::vector<wolf::sensor_id>& sensors) const;
  std::vector<wolf::room> handle_indoor_sensors(
      const std::vector<wolf::sensor_id>& sensors) const;
  std::vector<wolf::room> handle_outdoor_sensor(
      const std::vector<wolf::sensor_id>& sensors) const;
  static bool is_sensor_used(const wolf::room& room,
                             const wolf::sensor_id& check_sensor);
  static void remove_room_multiples(std::vector<wolf::room>& affected_rooms);
  static std::vector<wolf::sensor_id> get_sensor_ids_from_values(
      const std::vector<wolf::sensor_value>& values);
  static bool is_controller_sensor(const std::vector<wolf::sensor_id>& sensors);

  wolf::controller::controller_handler& m_controller_handler;
  wolf::actor::handler& m_actor_handler;
  wolf::room_handler& m_room_handler;
  wolf::sensor_value_handler& m_value_handler;
  configuration_handler& m_configuration_handler;
  wolf::sensor_value_offset_handler& m_offset_handler;
  configuration_values_handler& m_config_values_handler;
  wolf::profile_handler& m_profile_handler;
};

}  // namespace mold

#endif  // CONTROLLER_HANDLER_HPP
