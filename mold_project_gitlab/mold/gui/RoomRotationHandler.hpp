#ifndef MOLD_GUI_ROOMROTATIONHANDLER_HPP
#define MOLD_GUI_ROOMROTATIONHANDLER_HPP

#include "Co2StateHandler.hpp"
#include "WolfServices.hpp"

#include <QWidget>

namespace mold {

class RoomRotationHandler : public QWidget {
  Q_OBJECT

 public:
  RoomRotationHandler(const WolfServices& services,
                      const Co2StateHandler& co2_state,
                      QWidget* parent = nullptr);
  bool rotationLockedForRoom(const wolf::types::uuid_array& id,
                             const bool is_co2) const;
 signals:
  void jumpToRoom(const wolf::types::uuid_array& id, const bool is_co2);
  void rotationNotLocked(const wolf::types::uuid_array& id, const bool is_co2);

 private slots:
  void changeStateCo2(const wolf::types::uuid_array& room_id,
                      const calculation_state& state);

 private:
  void handleConfigStateChanged(const wolf::types::uuid_array& config_id,
                                const calculation_state& state);
  void handleRoomLockedChanged(const wolf::types::uuid_array& room_id,
                               const calculation_state& state,
                               const bool is_co2);
  struct lockedRoom {
    wolf::types::uuid_array id;
    bool is_co2;
  };
  using lockedRooms = std::vector<lockedRoom>;
  lockedRooms::const_iterator find(const wolf::types::uuid_array& id,
                                   const bool is_co2) const;
  bool eraseIfFound(const wolf::types::uuid_array& id, const bool is_co2);

 private:
  mutable logging::logger m_logger;
  const WolfServices& m_services;

  lockedRooms m_locked_rooms;
  boost::signals2::scoped_connection m_connection_config_states;
};
}  // namespace mold

#endif  // MOLD_GUI_ROOMROTATIONHANDLER_HPP
