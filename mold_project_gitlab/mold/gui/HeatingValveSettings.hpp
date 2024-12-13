#ifndef MOLD_GUI_HEATINGVALVESETTINGS_HPP
#define MOLD_GUI_HEATINGVALVESETTINGS_HPP

#include <QWidget>
#include "WolfServices.hpp"
#include "mold/heating_valve_helper.hpp"

namespace Ui {
class HeatingValveSettings;
}

namespace mold {

class HeatingValveSettings : public QWidget {
  Q_OBJECT

 public:
  explicit HeatingValveSettings(const WolfServices &services,
                                const wolf::room &room,
                                QWidget *parent = nullptr);
  ~HeatingValveSettings();
  std::optional<heating_valve_settings> get_current_settings() const;

 signals:
  void back();
  void openLearnPage();

 private slots:
  void enableManualMode();
  void setSliderValue(const int value);
  void handleCurrentValveDisplay();

 private:
  void saveSettings(std::function<void()> callOnFinished);
  void loadSettings();
  heating_valve_settings getSettingsFromUI() const;
  void handleLoadingFinished();
  void loadValues();
  void updateValues(const wolf::sensor_values &values);
  void setValuesToUI();

 private:
  std::unique_ptr<Ui::HeatingValveSettings> ui;

  logging::logger m_logger;

  const WolfServices &m_services;

  const wolf::room m_room;
  std::optional<heating_valve_settings> m_settings;
  heating_valve_helper::enocean_ids m_connected_valves;
  heating_valve_helper::sensor_ids m_sensor_ids;
  wolf::sensor_values m_values;
  unsigned int m_current_valve_index;

  boost::signals2::scoped_connection
      m_connection_values;  // from service thread to ui thread
};
}  // namespace mold

#endif  // MOLD_GUI_HEATINGVALVESETTINGS_HPP
