#ifndef MOLD_SENSORACTIVITYLOG_HPP
#define MOLD_SENSORACTIVITYLOG_HPP

#include <QWidget>

#include "DataExchange.hpp"
#include "SensorActivityHandler.hpp"

namespace Ui {
class SensorActivityLog;
}

namespace mold {

class SensorActivityLog : public QWidget {
  Q_OBJECT

 public:
  using sensor_activities = std::vector<SensorActivity>;

  explicit SensorActivityLog(SensorActivityHandler &activity_handler,
                             QWidget *parent = nullptr);
  ~SensorActivityLog();

 public slots:
  void handleSensorActivityChanged(const sensor_activities &sensors);
  void eraseList();

 signals:
  void back();

 private:
  void updateTable();

 private:
  std::unique_ptr<Ui::SensorActivityLog> ui;

  logging::logger m_logger;
  SensorActivityHandler &m_activity_handler;

  sensor_activities m_sensors;

  const int m_column_name;
  const int m_column_rssi;
  const int m_column_time;
};
}  // namespace mold

#endif  // MOLD_SENSORACTIVITYLOG_HPP
