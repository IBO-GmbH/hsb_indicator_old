#include "Co2StateHandler.hpp"

#include "PostToGUIThread.hpp"
#include "wolf/sensor_value_type_helper.hpp"

using namespace logging;

mold::Co2StateHandler::Co2StateHandler(const mold::WolfServices &services,
                                       QWidget *parent)
    : QWidget(parent),
      m_logger("Co2StateHandler"),
      m_services(services),
      m_co2_states{{{0, calculation_state::green},
                    {1000, calculation_state::yellow},
                    {1400, calculation_state::red}}} {
  m_connection_values = m_services.value_handler->signal_values.connect(
      [this](const auto &values) { handleValues(values); });
  const auto values = m_services.value_handler->get_all();
  if (values.size() > 0) handleValues(values);
}

void mold::Co2StateHandler::handleValues(const wolf::sensor_values &values) {
  for (const auto &value : values) handleValue(value);
}

mold::calculation_state mold::Co2StateHandler::getStateOfRoom(
    const wolf::types::uuid_array &id) const {
  LOGGING_LOG(m_logger, severity::verbose)
      << "get state of room with id: " << id;
  const auto found = const_find(id);
  if (found == m_room_states.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "room not found, state undefined";
    return calculation_state::undefined;
  }
  return found->state;
}

mold::Co2StateHandler::~Co2StateHandler() = default;

void mold::Co2StateHandler::handleValue(const wolf::sensor_value &value) {
  if (!wolf::sensor_value_type_helper::is_co2(value.id)) return;
  LOGGING_LOG(m_logger, severity::verbose) << "handling value: " << value;
  const auto state = getCalculationState(value.value);
  const auto room_ids = getRoomsWithSensor(value.id);
  postToGUIThread([this, room_ids, state, id = value.id] {
    for (const auto &room_id : room_ids) {
      const auto found = find(room_id);
      if (found == m_room_states.end()) {
        m_room_states.push_back({room_id, state});
        emit stateOfRoomChanged(room_id, state);
        continue;
      }
      if (found->state == state) continue;
      found->state = state;
      emit stateOfRoomChanged(room_id, state);
    }
    emit changeCo2Ui(id);
  });
}

mold::Co2StateHandler::roomIds mold::Co2StateHandler::getRoomsWithSensor(
    const wolf::sensor_id &sensor_id) const {
  const auto rooms = m_services.rooms->get_all();
  roomIds ids;
  for (const auto &room : rooms)
    for (const auto &sensor : room.sensors)
      if (sensor_id == sensor) ids.insert(room.id);
  return ids;
}

mold::calculation_state mold::Co2StateHandler::getCalculationState(
    const float value) const {
  for (std::array<range, 3>::const_reverse_iterator it = m_co2_states.rbegin();
       it != m_co2_states.rend(); ++it) {
    if (it->first <= value) return it->second;
  }
  return calculation_state::undefined;
}

mold::Co2StateHandler::RoomStates::iterator mold::Co2StateHandler::find(
    const wolf::types::uuid_array &id) {
  return std::find_if(m_room_states.begin(), m_room_states.end(),
                      [&id](const auto &room) { return room.id == id; });
}

mold::Co2StateHandler::RoomStates::const_iterator
mold::Co2StateHandler::const_find(const wolf::types::uuid_array &id) const {
  return std::find_if(m_room_states.begin(), m_room_states.end(),
                      [&id](const auto &room) { return room.id == id; });
}
