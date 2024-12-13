#include "RoomRotationHandler.hpp"
#include "PostToGUIThread.hpp"

using namespace logging;

mold::RoomRotationHandler::RoomRotationHandler(
    const mold::WolfServices &services, const Co2StateHandler &co2_state,
    QWidget *parent)
    : QWidget(parent), m_logger("RoomRotationHandler"), m_services(services) {
  m_connection_config_states =
      m_services.configuration_states->signal_state_changed.connect(
          [this](const auto &id, const auto &state) {
            handleConfigStateChanged(id, state);
          });
  connect(&co2_state, &Co2StateHandler::stateOfRoomChanged, this,
          &RoomRotationHandler::changeStateCo2);
}

void mold::RoomRotationHandler::changeStateCo2(
    const wolf::types::uuid_array &room_id,
    const mold::calculation_state &state) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle co2 state change for room with id: " << room_id
      << " to state: " << state;
  handleRoomLockedChanged(room_id, state, true);
}

bool mold::RoomRotationHandler::rotationLockedForRoom(
    const wolf::types::uuid_array &id, const bool is_co2) const {
  LOGGING_LOG(m_logger, severity::verbose)
      << "check if rotation locked for room with id: " << id;
  auto found = find(id, is_co2);
  return found != m_locked_rooms.end();
}

void mold::RoomRotationHandler::handleConfigStateChanged(
    const wolf::types::uuid_array &config_id,
    const mold::calculation_state &state) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle config state change for config with id: " << config_id
      << " to state: " << state;
  auto config = m_services.configurations->get(config_id);
  if (!config.has_value()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not find config with id: " << config_id;
    return;
  }
  auto room_id = config->room;
  handleRoomLockedChanged(room_id, state, false);
}

void mold::RoomRotationHandler::handleRoomLockedChanged(
    const wolf::types::uuid_array &room_id,
    const mold::calculation_state &state, const bool is_co2) {
  postToGUIThread([this, room_id, state, is_co2] {
    if (state != calculation_state::green) {
      if (!rotationLockedForRoom(room_id, is_co2))
        m_locked_rooms.push_back({room_id, is_co2});
      emit jumpToRoom(room_id, is_co2);
      return;
    }
    if (eraseIfFound(room_id, is_co2)) emit rotationNotLocked(room_id, is_co2);
  });
}

mold::RoomRotationHandler::lockedRooms::const_iterator
mold::RoomRotationHandler::find(const wolf::types::uuid_array &id,
                                const bool is_co2) const {
  return std::find_if(
      m_locked_rooms.begin(), m_locked_rooms.end(),
      [&id, &is_co2](const auto &locked_room) {
        return (locked_room.id == id && locked_room.is_co2 == is_co2);
      });
}

bool mold::RoomRotationHandler::eraseIfFound(const wolf::types::uuid_array &id,
                                             const bool is_co2) {
  auto found = find(id, is_co2);
  if (found == m_locked_rooms.end()) return false;
  m_locked_rooms.erase(found);
  return true;
}
