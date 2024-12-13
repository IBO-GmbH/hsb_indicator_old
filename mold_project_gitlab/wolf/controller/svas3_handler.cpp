#include "svas3_handler.hpp"

#include "wolf/actor/actor_indexes.hpp"
#include "wolf/actor/loop_usage_checker.hpp"

using namespace logging;

wolf::controller::svas3_handler::svas3_handler(
    wolf::controller::sensor_value_getter &value_getter)
    : m_logger("svas3_handler"), m_sensor_reader(value_getter) {}

void wolf::controller::svas3_handler::handle(
    const controller_per_room_list::iterator &found,
    const wolf::controller::svas3_handler::sensors &sensors,
    const float outdoor_temperature, std::stringstream &log_string) {
  if (!found->svas3->control_loop_is_active()) return;
  optional_float relative_humidity;
  optional_float temperature;
  const optional_float absolute_humidity =
      m_sensor_reader.get_absolute_humidity(sensors, relative_humidity,
                                            temperature);
  if (!absolute_humidity.has_value()) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "failed to get absolute humidity";
    return;
  }
  bool outdoor_temperature_right =
      outdoor_temperature >= found->limits_.outdoor_temperature;
  found->svas3->control(*absolute_humidity, relative_humidity.get_value_or(0),
                        temperature.get_value_or(0), outdoor_temperature_right);
  auto &all_actors = found->controller_->actors;
  auto actors_in_this_loop = actor::loop_usage_checker::get_actors_of_loop(
      all_actors, actor::ventilation_svas3, actor::window_drive_2_svas3);
  auto outputs = found->svas3->outputs_are_set();
  if (m_actor_setter.set_actors_svas3(found, outputs, log_string)) {
    signal_actors(actors_in_this_loop);
    return;
  }
  m_actor_setter.set_actors(
      found, {actor::ventilation_svas3, actor::window_drive_2_svas3}, false,
      log_string);
  signal_actors(actors_in_this_loop);
}
