#ifndef MOLD_GUI_CONTROLLERSETTINGS_HPP
#define MOLD_GUI_CONTROLLERSETTINGS_HPP

#include <QWidget>
#include <memory>

#include "ControllerTypes.hpp"
#include "log/logger.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/WolfServices.hpp"

namespace Ui {
class ControllerSettings;
}

namespace mold {

class ControllerSettings : public QWidget {
  Q_OBJECT

 public:
  using active_states = wolf::controller::values_per_room::active_states;

  explicit ControllerSettings(WolfServices &services,
                              DataExchange &dataExchange,
                              const ControllerTypes &type,
                              const wolf::types::uuid_array &room_id,
                              QWidget *parent = nullptr);
  ~ControllerSettings();

 private:
  active_states handleType(const bool active_state);
  void setActiveState(const bool checked);
  void handleStatesLoaded(const active_states &states);
  void loadActiveState();

 signals:
  void back();
  void changeActors();
  void setLimits();

 private:
  logging::logger m_logger;

  std::unique_ptr<Ui::ControllerSettings> ui;

  WolfServices &m_services;
  DataExchange &m_dataExchange;

  ControllerTypes m_type;
  wolf::types::uuid_array m_room_id;
  bool m_load_toggle;
};
}  // namespace mold
#endif  // MOLD_GUI_CONTROLLERSETTINGS_HPP
