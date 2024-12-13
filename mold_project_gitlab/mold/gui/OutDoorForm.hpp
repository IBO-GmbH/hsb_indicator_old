#ifndef MOLD_GUI_OUTDOORFORM_HPP
#define MOLD_GUI_OUTDOORFORM_HPP

#include "wolf/enocean_link.hpp"
#include "wolf/outdoor_sensor.hpp"
#include "wolf/sensor_profile.hpp"

#include <QWidget>

namespace Ui {
class OutDoorForm;
}

namespace mold {
class DataExchange;
class WolfServices;
}  // namespace mold

namespace wolf {
class esp3_parser;
}

namespace mold {

class OutDoorForm : public QWidget {
  Q_OBJECT

 public:
  explicit OutDoorForm(WolfServices &services, DataExchange &dataExchange,
                       QWidget *parent = nullptr);
  ~OutDoorForm();

 signals:
  void back();
  void chooseForeignSensor();
  void sensorOffset();

 private slots:
  void setSensorValuesToUi();
  void handleLearned(const wolf::types::id_esp3 &id, const std::uint8_t &org,
                     const std::uint8_t &func, const std::uint8_t &type);
  void handleForeignSensors();

 private:
  void handleEnoceanParsed(const wolf::enocean_link::parse_results &data);
  void handleBack();
  void saveToServices();
  void expandDescription(const bool temperature, const bool humidity,
                         const bool foreign, const bool foreign_list);
  void handleDescription(const bool foreign_list);
  void resizeAndRearrange(const bool show_values);
  void setSensorsInTheMiddle();

 private:
  std::unique_ptr<Ui::OutDoorForm> ui;

  logging::logger m_logger;

  WolfServices &m_services;
  DataExchange &m_dataExchange;
  wolf::enocean_link m_enocean;

  wolf::types::data m_last_data;
  wolf::types::id_esp3 m_last_enocean_id{};
  std::array<std::uint8_t, 3> m_last_eep;
  wolf::outdoor_sensor m_outdoor_id;
  int m_last_rssi{};
  bool m_first_time_values_to_ui{true};
  boost::optional<float> m_temperature;
  boost::optional<float> m_humidity;
  std::chrono::system_clock::time_point m_timepoint_values;

  QMetaObject::Connection m_connection_learn;
  boost::signals2::scoped_connection
      m_connection_enocean_data;  // from ui thread to ui thread
};
}  // namespace mold

#endif  // MOLD_GUI_OUTDOORFORM_HPP
