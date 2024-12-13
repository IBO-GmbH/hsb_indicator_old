#ifndef MOLD_GUI_CO2_STATE_HANDLER_HPP
#define MOLD_GUI_CO2_STATE_HANDLER_HPP

#include "WolfServices.hpp"

#include <QWidget>

namespace mold {

class Co2StateHandler : public QWidget {
  Q_OBJECT
 public:
  Co2StateHandler(const WolfServices& services, QWidget* parent = nullptr);
  void handleValues(const wolf::sensor_values& values);
  calculation_state getStateOfRoom(const wolf::types::uuid_array& id) const;
  ~Co2StateHandler();

 signals:
  void stateOfRoomChanged(const wolf::types::uuid_array& id,
                          const calculation_state& state);
  void changeCo2Ui(const wolf::sensor_id& id);

 private:
  void handleValue(const wolf::sensor_value& value);
  using roomIds = std::set<wolf::types::uuid_array>;
  roomIds getRoomsWithSensor(const wolf::sensor_id& sensor_id) const;
  calculation_state getCalculationState(const float value) const;
  struct RoomState {
    wolf::types::uuid_array id;
    calculation_state state;
  };
  using RoomStates = std::vector<RoomState>;
  RoomStates::iterator find(const wolf::types::uuid_array& id);
  RoomStates::const_iterator const_find(
      const wolf::types::uuid_array& id) const;

 private:
  mutable logging::logger m_logger;
  const WolfServices& m_services;

  using range = std::pair<int, calculation_state>;
  const std::array<range, 3> m_co2_states;
  RoomStates m_room_states;

  boost::signals2::scoped_connection m_connection_values;
};
}  // namespace mold

#endif  // MOLD_GUI_CO2_STATE_HANDLER_HPP
