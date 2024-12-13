#ifndef WOLF_CONTROLLER_SVAS4_HANDLER_HPP
#define WOLF_CONTROLLER_SVAS4_HANDLER_HPP

#include "controller_per_room.hpp"
#include "sensor_reader.hpp"
#include "sensor_value_getter.hpp"
#include "wolf/actor/actor_setter.hpp"
#include "wolf/sensor_id.hpp"

namespace wolf {

namespace controller {

class svas4_handler {
 public:
  using controller_per_room_list = std::vector<controller_per_room>;
  using sensors = std::vector<sensor_id>;
  using optional_float = boost::optional<float>;
  using optional_actor = boost::optional<actor::actor>;
  using actor_list_pointer = std::vector<optional_actor*>;

  svas4_handler(sensor_value_getter& value_getter);
  void handle(const controller_per_room_list::iterator& found,
              const sensors& sensors, const float outdoor_temperature,
              std::stringstream& log_string);

  boost::signals2::signal<void(const actor_list_pointer&)> signal_actors;

 private:
  logging::logger m_logger;
  actor::actor_setter m_actor_setter;
  sensor_reader m_sensor_reader;
};
}  // namespace controller
}  // namespace wolf
#endif  // WOLF_CONTROLLER_SVAS4_HANDLER_HPP
