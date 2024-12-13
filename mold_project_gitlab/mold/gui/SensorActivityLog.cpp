#include "SensorActivityLog.hpp"
#include "ui_SensorActivityLog.h"

#include <QDateTime>
#include <QScrollBar>

using namespace mold;

SensorActivityLog::SensorActivityLog(SensorActivityHandler& activity_handler,
                                     QWidget* parent)
    : QWidget(parent),
      ui(new Ui::SensorActivityLog),
      m_logger("SensorActivityLog"),
      m_activity_handler(activity_handler),
      m_column_name(0),
      m_column_rssi(1),
      m_column_time(2) {
  ui->setupUi(this);

  connect(&m_activity_handler, &SensorActivityHandler::sensorActivityChanged,
          this, &SensorActivityLog::handleSensorActivityChanged);

  m_sensors = m_activity_handler.getSensorActivites();

  connect(ui->back, &QPushButton::clicked, this, &SensorActivityLog::back);
  connect(ui->erase, &QPushButton::clicked, this,
          &SensorActivityLog::eraseList);

  ui->sensors->setColumnWidth(m_column_rssi, 35);
  ui->sensors->setColumnWidth(m_column_time, 100);
  updateTable();
}

void SensorActivityLog::handleSensorActivityChanged(
    const sensor_activities& sensors) {
  m_sensors = sensors;
  updateTable();
}

void SensorActivityLog::eraseList() {
  ui->sensors->clearContents();
  m_activity_handler.eraseSensorActivities();
  update();
}

void SensorActivityLog::updateTable() {
  ui->sensors->setRowCount(static_cast<int>(m_sensors.size()));
  ui->sensors->setSortingEnabled(false);
  for (unsigned int row = 0; row < m_sensors.size(); ++row) {
    ui->sensors->setRowHeight(static_cast<int>(row), 15);
    QTableWidgetItem* item =
        new QTableWidgetItem(QString::fromStdString(m_sensors[row].room_name));
    item->setFlags(Qt::ItemIsEnabled);
    ui->sensors->setItem(static_cast<int>(row), m_column_name, item);
    item = new QTableWidgetItem(
        QString::fromStdString(std::to_string(m_sensors[row].rssi)));
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::ItemIsEnabled);
    ui->sensors->setItem(static_cast<int>(row), m_column_rssi, item);
    auto time = std::chrono::system_clock::to_time_t(m_sensors[row].time);
    auto date_time = QDateTime::fromSecsSinceEpoch(time);
    item = new QTableWidgetItem(date_time.toString("dd.MM.yy HH:mm:ss"));
    item->setFlags(Qt::ItemIsEnabled);
    ui->sensors->setItem(static_cast<int>(row), m_column_time, item);
  }
  ui->sensors->horizontalHeader()->setSectionResizeMode(m_column_name,
                                                        QHeaderView::Stretch);
  ui->sensors->setSortingEnabled(true);
  ui->sensors->sortItems(m_column_time, Qt::SortOrder::DescendingOrder);
}

SensorActivityLog::~SensorActivityLog() = default;
