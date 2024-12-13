#include "controller_handler.hpp"
#include "calculator.hpp"
#include "type.hpp"
#include "wolf/actor/loop_usage_checker.hpp"
#include "wolf/thread_checker.hpp"

using namespace logging;

wolf::controller::controller_handler::controller_handler(
    wolf::sensor_value_handler &value_handler, controller_cache *cache,
    const config &program_options)
    : m_logger("controller_handler"),
      m_cache(cache),
      m_program_options(program_options),
      m_value_getter(value_handler),
      m_threshold_setter(program_options),
      m_outdoor_temperature(
          m_program_options.controller_.invalid_outdoor_temperature),
      m_svas1_handler(m_value_getter),
      m_svas2_handler(m_value_getter),
      m_svas3_handler(m_value_getter),
      m_svas4_handler(m_value_getter) {
  m_svas1_handler.signal_actors.connect(
      [this](const actor_list_pointer &actors) { signal_actor_state(actors); });
  m_svas2_handler.signal_actors.connect(
      [this](const actor_list_pointer &actors) { signal_actor_state(actors); });
  m_svas3_handler.signal_actors.connect(
      [this](const actor_list_pointer &actors) { signal_actor_state(actors); });
  m_svas4_handler.signal_actors.connect(
      [this](const actor_list_pointer &actors) { signal_actor_state(actors); });
  load_from_database();
}

bool wolf::controller::controller_handler::add(
    const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "add controller values to room with id: " << room_id;
  auto found = find(room_id);
  if (found != m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "add failed because room already added id:" << room_id;
    return false;
  }

  auto entry = make_new_entry(room_id);
  m_controller_list.push_back(entry);
  auto states = get_active_states(room_id);
  auto actors = entry.controller_->actors;
  if (m_cache) m_cache->add({room_id, entry.limits_, actors, states});
  set_limits(room_id, entry.limits_, true);
  signal_update_gui(get_all_values());
  return true;
}

bool wolf::controller::controller_handler::update(
    const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "update controller values in room with id: " << room_id;
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "update failed because room not found id:" << room_id;
    return false;
  }
  auto limits = get_limits(room_id);
  auto states = get_active_states(room_id);
  auto actors = found->controller_->actors;
  if (m_cache) m_cache->update({room_id, limits, actors, states});
  signal_update_gui(get_all_values());
  return true;
}

bool wolf::controller::controller_handler::remove(
    const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "remove controller values from room with id: " << room_id;
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "remove failed because room not found id:" << room_id;
    return false;
  }
  m_controller_list.erase(found);
  if (m_cache) m_cache->remove(room_id);
  return true;
}

void wolf::controller::controller_handler::set_outdoor_sensor(
    const outdoor_sensor &outdoor_sensor) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "setting outdoor sensor to: " << outdoor_sensor;
  m_outdoor = outdoor_sensor;
}

bool wolf::controller::controller_handler::set_limits(
    const wolf::types::uuid_array &room_id,
    const wolf::controller::limits &limits, const bool initial) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set limits for room with id: " << room_id << ", " << limits;
  wolf::thread_checker::throw_if_not_service("controller_handler::set_limits");
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "set_limits failed because room not found id:" << room_id;
    return false;
  }
  found->limits_ = limits;
  auto absolute_humidity = calculate_absolute_humidity_thresholds();
  m_threshold_setter.set_all(found, limits, absolute_humidity);
  if (!initial) signal_limits(room_id);
  return true;
}

wolf::controller::limits wolf::controller::controller_handler::get_limits(
    const wolf::types::uuid_array &room_id) {
  wolf::thread_checker::throw_if_not_service("controller_handler::get_limits");
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "add failed because room not found id:" << room_id;
    throw std::runtime_error("get_limits room not available");
  }
  return found->limits_;
}

static bool is_true(bool check) { return check; }

bool wolf::controller::controller_handler::handle_control_loops(
    const room &room, const bool settings_changed) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "start control loops for room with id: " << room.id;
  const auto found = find(room.id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "handle_control_loops failed because room not found id:" << room.id;
    return false;
  }
  wolf::controller::controller_handler::active_states states =
      get_active_states(room.id);
  if (std::none_of(states.cbegin(), states.cend(), is_true)) {
    if (settings_changed) {
      deactivate_all_actors_in_room(room.id);
      return update(room.id);
    }
    return true;
  }
  handle_all(found, room.sensors);
  return update(room.id);
}

bool wolf::controller::controller_handler::set_active_states(
    const wolf::types::uuid_array &room_id,
    const wolf::controller::controller_handler::active_states &states) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set control loop states for room with id: " << room_id
      << ", states: " << states[0] << ", " << states[1] << ", " << states[2]
      << ", " << states[3];

  wolf::thread_checker::throw_if_not_service(
      "controller_handler::set_active_states");

  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "set_active_states failed because room not found id:" << room_id;
    return false;
  }

  found->svas1->set_control_loop_active(states[0]);
  found->svas2->set_control_loop_active(states[1]);
  found->svas3->set_control_loop_active(states[2]);
  found->svas4->set_control_loop_active(states[3]);
  signal_active_sates(room_id);
  return true;
}

wolf::controller::controller_handler::active_states
wolf::controller::controller_handler::get_active_states(
    const wolf::types::uuid_array &room_id) const {
  wolf::thread_checker::throw_if_not_service(
      "controller_handler::get_active_states");
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "get_active_states failed because room not found id:" << room_id;
    throw std::runtime_error("get_active_states room not available");
  }
  active_states states;
  states[0] = found->svas1->control_loop_is_active();
  states[1] = found->svas2->control_loop_is_active();
  states[2] = found->svas3->control_loop_is_active();
  states[3] = found->svas4->control_loop_is_active();
  return states;
}

bool wolf::controller::controller_handler::add_actor(
    const wolf::types::uuid_array &room_id, const wolf::actor::actor &to_add,
    const unsigned int actor_index) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "add actor to room with id: " << room_id << ", actor id: " << std::dec
      << to_add.id;
  wolf::thread_checker::throw_if_not_service("controller_handler::add_actor");
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "add_actor failed because room not found id:" << room_id;
    return false;
  }
  check_actor(found, to_add, actor_index);
  signal_add_actor(room_id, to_add);
  return true;
}

bool wolf::controller::controller_handler::remove_actors(
    const wolf::types::uuid_array &room_id,
    const actor::indexes &actor_indexes) {
  if (actor_indexes.start > actor_indexes.end) {
    LOGGING_LOG(m_logger, severity::error)
        << "start index bigger than end index";
    return false;
  }
  LOGGING_LOG(m_logger, severity::verbose)
      << "remove actors from room with id: " << room_id;
  wolf::thread_checker::throw_if_not_service(
      "controller_handler::remove_actors");
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "remove_actors failed because room not found id:" << room_id;
    return false;
  }

  handle_remove_actor(found, room_id, actor_indexes);
  signal_all_actors_removed(room_id);
  return true;
}

wolf::controller::controller::actor_array
wolf::controller::controller_handler::get_actors_list(
    const wolf::types::uuid_array &room_id) {
  wolf::thread_checker::throw_if_not_service(
      "controller_handler::get_actors_list");
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "get_actors_list failed because room not found id:" << room_id;
    throw std::runtime_error("get_actors_list room not available");
  }
  return found->controller_->actors;
}

wolf::controller::controller_handler::controller_per_room_list
wolf::controller::controller_handler::get_all() {
  return m_controller_list;
}

wolf::controller::controller_handler::values_list
wolf::controller::controller_handler::get_all_values() {
  if (m_cache) return m_cache->get_all();
  return {};
}

void wolf::controller::controller_handler::load_from_database() {
  LOGGING_LOG(m_logger, severity::verbose) << "load values from database";
  if (!m_cache) return;
  auto list = m_cache->get_all();
  if (list.empty()) {
    LOGGING_LOG(m_logger, severity::verbose) << "nothing to load";
    return;
  }
  for (auto &value : list) {
    auto entry = make_new_entry(value.room_id);
    entry.limits_ = value.limits_;
    entry.controller_->actors = value.actors;
    m_controller_list.push_back(entry);
    auto absolute_humidity = calculate_absolute_humidity_thresholds();
    auto position = m_controller_list.end() - 1;
    m_threshold_setter.set_all(position, value.limits_, absolute_humidity);
    set_active_states(entry.room_id, value.active_loops);
  }
}

void wolf::controller::controller_handler::reset_all_actors() {
  for (auto &values : m_controller_list)
    for (auto &actor : values.controller_->actors) {
      if (actor) {
        actor.get().state = false;
        signal_load_actor(actor.get());
      }
      continue;
    }
}

bool wolf::controller::controller_handler::reset_ventilation(
    const wolf::types::uuid_array &room_id) {
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "reset_ventilation failed because room not found id:" << room_id;
    return false;
  }
  found->ventilation->reset_ventilation();
  found->svas1->reset_ventilation();
  found->svas2->reset_ventilation();
  found->svas3->reset_ventilation();
  return true;
}

bool wolf::controller::controller_handler::reset_heating(
    const wolf::types::uuid_array &room_id) {
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "reset_heating failed because room not found id:" << room_id;
    return false;
  }
  found->svas3->reset_heating();
  return true;
}

bool wolf::controller::controller_handler::reset_dehumidifier(
    const wolf::types::uuid_array &room_id) {
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "reset_dehumifier failed because room not found id:" << room_id;
    return false;
  }
  found->svas2->reset_dehumidifier();
  return true;
}

void wolf::controller::controller_handler::update_absolute_humidity_thresholds(
    const wolf::types::uuid_array &room_id) {
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "update_absolute_humidity_thresholds failed because room not found "
           "id:"
        << room_id;
    return;
  }
  auto absolute_humidity = calculate_absolute_humidity_thresholds();
  m_threshold_setter.set_absolute_humidity(found, absolute_humidity);
  signal_limits(room_id);
}

void wolf::controller::controller_handler::check_actor(
    const controller_per_room_list::iterator &found,
    const wolf::actor::actor &to_add, const unsigned int actor_index) {
  auto &all_actors = found->controller_->actors;
  auto &actor_at_index = all_actors[actor_index];
  if (actor_at_index.has_value()) {
    auto id_old = found->controller_->actors[actor_index].get().id;
    if (!actor::loop_usage_checker::used_in_other_loops(all_actors,
                                                        actor_at_index.get()))
      signal_remove_actor(id_old);
  }
  found->controller_->actors[actor_index] = to_add;
}

wolf::controller::controller_per_room
wolf::controller::controller_handler::make_new_entry(
    const wolf::types::uuid_array &room_id) {
  limits limits_;
  auto controller_ = std::make_shared<controller>();
  auto &controls = controller_->controller_;
  auto ventilation = std::make_shared<controller_ventilation>(controls);
  auto svas1 = std::make_shared<svas1_ventilation>(ventilation);
  auto svas2 = std::make_shared<svas2_dehumidifier>(ventilation);
  auto svas3 = std::make_shared<svas3_heating>(ventilation);
  auto svas4 = std::make_shared<svas4_air_quality>(controls);
  return {room_id, limits_, controller_, ventilation,
          svas1,   svas2,   svas3,       svas4};
}

wolf::controller::controller_handler::controller_per_room_list::iterator
wolf::controller::controller_handler::find(
    const wolf::types::uuid_array &room_id) {
  return std::find_if(m_controller_list.begin(), m_controller_list.end(),
                      [&room_id](const controller_per_room &values) {
                        return values.room_id == room_id;
                      });
}

wolf::controller::controller_handler::controller_per_room_list::const_iterator
wolf::controller::controller_handler::find(
    const wolf::types::uuid_array &room_id) const {
  return std::find_if(m_controller_list.cbegin(), m_controller_list.cend(),
                      [&room_id](const controller_per_room &values) {
                        return values.room_id == room_id;
                      });
}

wolf::controller::thresholds
wolf::controller::controller_handler::calculate_absolute_humidity_thresholds() {
  const auto humidity_sensor = m_outdoor.humidity;
  const auto temperature_sensor = m_outdoor.temperature;
  const auto humidity = m_value_getter.get_humidity(humidity_sensor);
  const auto temperature = m_value_getter.get_temperature(temperature_sensor);
  if (!humidity) {
    if (temperature) m_outdoor_temperature = temperature.get();
    return {0.f, 0.f};
  }
  m_outdoor_temperature = temperature.get();
  auto absolute_humidity = calculator::calculate_absolute_humidity(
      m_outdoor_temperature, humidity.get());
  if (absolute_humidity >
      m_program_options.controller_.max_absolute_humidity_outdoor)
    return {0.f, 0.f};
  absolute_humidity +=
      m_program_options.controller_.difference_absolute_humidity;
  return {m_program_options.controller_.absolute_humidity_threshold,
          absolute_humidity};
}

void wolf::controller::controller_handler::handle_svas1(
    const controller_per_room_list::iterator &found, const sensors &sensors,
    const float outdoor_temperature, std::stringstream &log_string) {
  m_svas1_handler.handle(found, sensors, outdoor_temperature, log_string);
}

void wolf::controller::controller_handler::handle_svas2(
    const controller_per_room_list::iterator &found, const sensors &sensors,
    const float outdoor_temperature, std::stringstream &log_string) {
  m_svas2_handler.handle(found, sensors, outdoor_temperature, log_string);
}

void wolf::controller::controller_handler::handle_svas3(
    const controller_per_room_list::iterator &found, const sensors &sensors,
    const float outdoor_temperature, std::stringstream &log_string) {
  m_svas3_handler.handle(found, sensors, outdoor_temperature, log_string);
}

void wolf::controller::controller_handler::handle_svas4(
    const controller_per_room_list::iterator &found, const sensors &sensors,
    const float outdoor_temperature, std::stringstream &log_string) {
  m_svas4_handler.handle(found, sensors, outdoor_temperature, log_string);
}

void wolf::controller::controller_handler::handle_all(
    const controller_per_room_list::iterator &found,
    const wolf::controller::controller_handler::sensors &sensors) {
  std::stringstream log_string;
  handle_svas1(found, sensors, m_outdoor_temperature, log_string);
  handle_svas2(found, sensors, m_outdoor_temperature, log_string);
  handle_svas3(found, sensors, m_outdoor_temperature, log_string);
  handle_svas4(found, sensors, m_outdoor_temperature, log_string);
  const std::string log_entry = log_string.str();
  if (!log_entry.empty())
    LOGGING_LOG(m_logger, severity::verbose)
        << "no actor at index: " << log_string.rdbuf();
}

void wolf::controller::controller_handler::deactivate_all_actors_in_room(
    const wolf::types::uuid_array &room_id) {
  auto found = find(room_id);
  if (found == m_controller_list.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "deactivate_all_actors_in_room failed because room not found id:"
        << room_id;
    return;
  }
  auto &actors = found->controller_->actors;
  for (auto &actor : actors)
    if (actor) actor.get().state = false;
  auto actors_vector = convert_array_to_vector(actors);
  signal_actor_state(actors_vector);
}

wolf::controller::controller_handler::actor_list_pointer
wolf::controller::controller_handler::convert_array_to_vector(
    wolf::controller::controller::actor_array &actors) {
  actor_list_pointer list;
  for (auto &actor : actors) list.push_back(&actor);
  return list;
}

void wolf::controller::controller_handler::handle_remove_actor(
    const controller_per_room_list::iterator &found,
    const wolf::types::uuid_array &room_id,
    const actor::indexes &actor_indexes) {
  auto &all_actors = found->controller_->actors;
  for (unsigned int index = actor_indexes.start; index <= actor_indexes.end;
       ++index) {
    auto &actor_at_index = all_actors[index];
    if (actor_at_index.has_value()) {
      if (!actor::loop_usage_checker::used_in_other_rooms(
              m_controller_list, room_id, actor_at_index.get()) &&
          !actor::loop_usage_checker::used_in_other_loops(all_actors,
                                                          actor_at_index.get()))
        signal_remove_actor(actor_at_index.get().id);
    }
    actor_at_index = boost::none;
  }
}
