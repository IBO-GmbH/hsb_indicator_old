#ifndef MOLD_GUI_MOLDVALUESFORM_HPP
#define MOLD_GUI_MOLDVALUESFORM_HPP

#include <QWidget>

#include "mold/configuration.hpp"
#include "mold/configuration_state.hpp"
#include "mold/configuration_values_outdoor.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/WolfServices.hpp"
#include "wolf/room.hpp"

namespace Ui {
class MoldValuesForm;
}
class QLabel;

namespace mold {

class MoldValuesForm : public QWidget {
  Q_OBJECT

 public:
  using timestamp = std::chrono::system_clock::time_point;
  using optional_value = boost::optional<configuration_values::value>;
  using duration = std::chrono::system_clock::duration;

  explicit MoldValuesForm(const WolfServices &services,
                          const DataExchange &dataExchange,
                          const wolf::types::uuid_array &room_id,
                          QWidget *parent = nullptr);
  ~MoldValuesForm();

  void updateOutputWithCatch();
  void updateOutput();
  const configuration_values &getIndoorValues() const;
  const configuration_values_outdoor &getOutdoorValues() const;

 private:
  bool updateValuesUI(const timestamp &now);
  void updateOutdoorValueUI(const timestamp &now);
  void updateStateUI(const bool did_calculation_state_change,
                     const bool humidity_old);
  void checkColoursTemperatureIndoor(const timestamp &now);
  bool checkColoursHumidityIndoor(const timestamp &now);
  void checkColoursTemperatureOutdoor(const timestamp &now);
  void checkColoursHumidityOutdoor(const timestamp &now);

 signals:
  void updateLine();
  void updateValue();
  void openHeatingValveOptions();

 private:
  std::unique_ptr<Ui::MoldValuesForm> ui;

  logging::logger m_logger;

  const WolfServices &m_services;
  const DataExchange &m_dataExchange;

  float m_mold_value;

  const wolf::types::uuid_array m_room_id;
  wolf::room m_room;
  configuration m_configuration;
  configuration_values m_values;
  configuration_state m_state;

  using outdoor_values_type = configuration_values_outdoor;
  outdoor_values_type m_values_outdoor;

  const duration m_calculation_timeout =
      std::chrono::minutes(m_services.config->mold_states_.calculation_timeout);
  const duration m_sensor_red_time = std::chrono::minutes(
      m_services.config_wolf->general_.sensor_values_grey_time);
};
}  // namespace mold

#endif  // MOLD_GUI_MOLDVALUESFORM_HPP
