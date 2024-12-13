#ifndef MOLD_SENSORACTIVITYHANDLER_HPP
#define MOLD_SENSORACTIVITYHANDLER_HPP

#include "DataExchange.hpp"
#include "SensorActivity.hpp"
#include "wolf/outdoor_sensor.hpp"
#include "wolf/program_options_parser.hpp"
#include "wolf/room_handler.hpp"

#include <QWidget>

namespace mold {

class SensorActivityHandler : public QWidget {
  Q_OBJECT

 public:
  using sensor_activities = std::vector<SensorActivity>;
  using rooms = std::vector<wolf::room>;

  SensorActivityHandler(const wolf::config &program_options,
                        const wolf::outdoor_sensor &outdoor_sensor,
                        const DataExchange &dataExchange,
                        QWidget *parent = nullptr);
  sensor_activities getSensorActivites() const;
  void eraseSensorActivities();

 signals:
  void sensorActivityChanged(const sensor_activities &activities);

 private slots:
  void handleSensorActivity(const wolf::sensor_id &id, const int rssi,
                            const rooms &rooms_);
  void changeOutdoorSensor(const wolf::outdoor_sensor &outdoor_sensor);

 private:
  void addSensor(const wolf::sensor_id &id, const int rssi,
                 const rooms &rooms_);
  void removeOldSensor();
  boost::optional<std::string> getRoomName(const wolf::sensor_id &id,
                                           const rooms &rooms_);
  bool isOutdoor(const wolf::sensor_id &id);
  bool compareSensors(const wolf::sensor_id &id_1, const wolf::sensor_id &id_2);

 private:
  logging::logger m_logger;
  sensor_activities m_sensors;

  const int m_sensor_count;
  wolf::outdoor_sensor m_outdoor;

  boost::signals2::scoped_connection m_connection_learned;
};
}  // namespace mold

#endif  // MOLD_SENSORACTIVITYHANDLER_HPP
