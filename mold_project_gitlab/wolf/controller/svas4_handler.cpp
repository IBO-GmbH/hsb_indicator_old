#include "svas4_handler.hpp"

#include "wolf/actor/actor_indexes.hpp"
#include "wolf/actor/loop_usage_checker.hpp"

using namespace logging;

wolf::controller::svas4_handler::svas4_handler(
    wolf::controller::sensor_value_getter &value_getter)
    : m_logger("svas4_handler"), m_sensor_reader(value_getter) {}

void wolf::controller::svas4_handler::handle(
    const controller_per_room_list::iterator &found,
    const wolf::controller::svas4_handler::sensors &sensors,
    const float outdoor_temperature, std::stringstream &log_string) {
  if (!found->svas4->control_loop_is_active()) return;
  optional_float co2, sprout_value, voc;
  if (!m_sensor_reader.get_sensor_values_svas4(sensors, co2, sprout_value,
                                               voc)) {
    LOGGING_LOG(m_logger, severity::verbose) << "failed to get sensor values";
    return;
  }
  bool outdoor_temperature_right =
      outdoor_temperature >= found->limits_.outdoor_temperature;
  found->svas4->control(co2.get_value_or(0), sprout_value.get_value_or(0),
                        voc.get_value_or(0), outdoor_temperature_right);
  auto &all_actors = found->controller_->actors;
  auto actors_in_this_loop = actor::loop_usage_checker::get_actors_of_loop(
      all_actors, actor::ventilation_svas4, actor::window_drive_3_svas4);
  m_actor_setter.set_actors(
      found, {actor::ventilation_svas4, actor::window_drive_3_svas4},
      found->svas4->output_is_set(), log_string);
  signal_actors(actors_in_this_loop);
}
