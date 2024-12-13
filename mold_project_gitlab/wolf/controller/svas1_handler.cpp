#include "svas1_handler.hpp"

#include "wolf/actor/actor_indexes.hpp"
#include "wolf/actor/loop_usage_checker.hpp"

using namespace logging;

wolf::controller::svas1_handler::svas1_handler(
    wolf::controller::sensor_value_getter &value_getter)
    : m_logger("svas1_handler"), m_sensor_reader(value_getter) {}

void wolf::controller::svas1_handler::handle(
    const controller_per_room_list::iterator &found,
    const wolf::controller::svas1_handler::sensors &sensors,
    const float outdoor_temperature, std::stringstream &log_string) {
  if (!found->svas1->control_loop_is_active()) return;
  optional_float relative_humidity;
  const optional_float absolute_humidity =
      m_sensor_reader.get_absolute_humidity(sensors, relative_humidity);
  if (!absolute_humidity.has_value()) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "failed to get absolute humidity";
    return;
  }
  bool outdoor_temperature_right =
      outdoor_temperature >= found->limits_.outdoor_temperature;
  found->svas1->control(absolute_humidity.get(),
                        relative_humidity.get_value_or(0),
                        outdoor_temperature_right);
  auto &all_actors = found->controller_->actors;
  auto actors_in_this_loop = actor::loop_usage_checker::get_actors_of_loop(
      all_actors, actor::ventilation_svas1, actor::window_drive_3_svas1);
  m_actor_setter.set_actors(
      found, {actor::ventilation_svas1, actor::window_drive_3_svas1},
      found->svas1->output_is_set(), log_string);
  signal_actors(actors_in_this_loop);
}
