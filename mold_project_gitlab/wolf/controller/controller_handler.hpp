#ifndef WOLF_CONTROLLER_CONTROLLER_HANDLER_HPP
#define WOLF_CONTROLLER_CONTROLLER_HANDLER_HPP

#include "controller_cache.hpp"
#include "svas1_handler.hpp"
#include "svas2_handler.hpp"
#include "svas3_handler.hpp"
#include "svas4_handler.hpp"
#include "threshold_setter.hpp"
#include "wolf/actor/indexes.hpp"
#include "wolf/outdoor_sensor.hpp"
#include "wolf/room.hpp"

#include <chrono>

namespace wolf {

namespace controller {

class controller_handler {
 public:
  using wait_time = std::chrono::system_clock::duration;
  using active_states = std::array<bool, controller::control_loops_per_room>;
  using sensors = std::vector<sensor_id>;
  using optional_float = boost::optional<float>;
  using actor_list_pointer = std::vector<boost::optional<actor::actor>*>;

  controller_handler(sensor_value_handler& sensor_value_handler,
                     controller_cache* cache,
                     const wolf::config& program_options);
  bool add(const types::uuid_array& room_id);
  bool update(const types::uuid_array& room_id);
  bool remove(const types::uuid_array& room_id);
  void set_outdoor_sensor(const outdoor_sensor& outdoor_sensor);
  bool set_limits(const types::uuid_array& room_id, const limits& limits,
                  const bool initial = false);
  limits get_limits(const types::uuid_array& room_id);
  bool handle_control_loops(const room& room, const bool settings_changed);
  bool set_active_states(const types::uuid_array& room_id,
                         const active_states& states);
  active_states get_active_states(const types::uuid_array& room_id) const;
  bool add_actor(const types::uuid_array& room_id, const actor::actor& to_add,
                 const unsigned int actor_index);
  bool remove_actors(const types::uuid_array& room_id,
                     const actor::indexes& actor_indexes);
  controller::actor_array get_actors_list(const types::uuid_array& room_id);
  void reset_all_actors();
  bool reset_ventilation(const types::uuid_array& room_id);
  bool reset_heating(const types::uuid_array& room_id);
  bool reset_dehumidifier(const types::uuid_array& room_id);
  void update_absolute_humidity_thresholds(
      const wolf::types::uuid_array& room_id);

  using controller_per_room_list = std::vector<controller_per_room>;
  controller_per_room_list get_all();

  using values_list = std::vector<values_per_room>;
  values_list get_all_values();

  boost::signals2::signal<void(const values_list&)> signal_update_gui;
  boost::signals2::signal<void(const types::uuid_array&)> signal_limits;
  boost::signals2::signal<void(const types::uuid_array&)> signal_active_sates;
  boost::signals2::signal<void(const types::uuid_array&, const actor::actor&)>
      signal_add_actor;
  boost::signals2::signal<void(const actor::actor&)> signal_load_actor;
  boost::signals2::signal<void(const types::id_esp3&)> signal_remove_actor;
  boost::signals2::signal<void(const types::uuid_array&)>
      signal_all_actors_removed;
  boost::signals2::signal<void(const actor_list_pointer&)> signal_actor_state;

 private:
  void load_from_database();
  void check_actor(const controller_per_room_list::iterator& found,
                   const actor::actor& to_add, const unsigned int actor_index);
  controller_per_room make_new_entry(const types::uuid_array& room_id);
  controller_per_room_list::iterator find(const types::uuid_array& room_id);
  controller_per_room_list::const_iterator find(
      const types::uuid_array& room_id) const;
  thresholds calculate_absolute_humidity_thresholds();
  void handle_svas1(const controller_per_room_list::iterator& found,
                    const sensors& sensors, const float outdoor_temperature,
                    std::stringstream& log_string);
  void handle_svas2(const controller_per_room_list::iterator& found,
                    const sensors& sensors, const float outdoor_temperature,
                    std::stringstream& log_string);
  void handle_svas3(const controller_per_room_list::iterator& found,
                    const sensors& sensors, const float outdoor_temperature,
                    std::stringstream& log_string);
  void handle_svas4(const controller_per_room_list::iterator& found,
                    const sensors& sensors, const float outdoor_temperature,
                    std::stringstream& log_string);
  void handle_all(const controller_per_room_list::iterator& found,
                  const sensors& sensors);
  void deactivate_all_actors_in_room(const types::uuid_array& room_id);
  actor_list_pointer convert_array_to_vector(controller::actor_array& actors);
  void handle_remove_actor(const controller_per_room_list::iterator& found,
                           const types::uuid_array& room_id,
                           const actor::indexes& actor_indexes);

 private:
  mutable logging::logger m_logger;
  controller_cache* m_cache;
  const config m_program_options;
  sensor_value_getter m_value_getter;
  controller_per_room_list m_controller_list;
  outdoor_sensor m_outdoor;
  threshold_setter m_threshold_setter;
  float m_outdoor_temperature;
  svas1_handler m_svas1_handler;
  svas2_handler m_svas2_handler;
  svas3_handler m_svas3_handler;
  svas4_handler m_svas4_handler;
};
}  // namespace controller
}  // namespace wolf
#endif  // WOLF_CONTROLLER_CONTROLLER_HANDLER_HPP
