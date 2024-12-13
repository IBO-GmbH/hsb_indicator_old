#include "SensorActivityHandler.hpp"
#include <QString>
#include "PostToGUIThread.hpp"
#include "wolf/program_options_parser.hpp"
#include "wolf/sensor_id_rssi.hpp"

using namespace mold;
using namespace logging;

SensorActivityHandler::SensorActivityHandler(
    const wolf::config &program_options,
    const wolf::outdoor_sensor &outdoor_sensor,
    const DataExchange &dataExchange, QWidget *parent)
    : QWidget(parent),
      m_logger("SensorActivityHandler"),
      m_sensor_count(program_options.general_.sensor_activity_count),
      m_outdoor(outdoor_sensor) {
  connect(&dataExchange, &DataExchange::configuration_outdoor_sensor_changed,
          this, &SensorActivityHandler::changeOutdoorSensor);
  connect(&dataExchange, &DataExchange::enocean_values_read, this,
          &SensorActivityHandler::handleSensorActivity);
  auto parser = dataExchange.getServices().esp3_parser;
  if (parser != nullptr) {
    m_connection_learned = parser->signal_data_learned.connect(
        [this](const auto &id, const auto &eep, const int rssi) {
          postToGUIThread([this, id, eep, rssi] {
            const auto sensor_id =
                wolf::sensor_id_rssi::create_from_esp3_id(id);
            std::stringstream room_name;
            room_name << "L: ";
            for (const auto &eep_part : eep) {
              room_name << std::hex << +eep_part << "-";
            }
            room_name << std::hex << id;
            handleSensorActivity(sensor_id, rssi,
                                 {{{}, room_name.str(), {sensor_id}}});
          });
        });
  }
}

void SensorActivityHandler::handleSensorActivity(
    const wolf::sensor_id &id, const int rssi,
    const SensorActivityHandler::rooms &rooms_) {
  addSensor(id, rssi, rooms_);
}

void SensorActivityHandler::changeOutdoorSensor(
    const wolf::outdoor_sensor &outdoor_sensor) {
  m_outdoor = outdoor_sensor;
}

SensorActivityHandler::sensor_activities
SensorActivityHandler::getSensorActivites() const {
  return m_sensors;
}

void SensorActivityHandler::eraseSensorActivities() { m_sensors.clear(); }

void SensorActivityHandler::addSensor(
    const wolf::sensor_id &id, const int rssi,
    const SensorActivityHandler::rooms &rooms_) {
  if (m_sensors.size() > static_cast<unsigned int>(m_sensor_count))
    removeOldSensor();
  auto name = getRoomName(id, rooms_);
  if (!name) return;
  m_sensors.push_back({name.get(), rssi, std::chrono::system_clock::now()});
  emit sensorActivityChanged(m_sensors);
}

void SensorActivityHandler::removeOldSensor() {
  auto found = std::min_element(
      m_sensors.begin(), m_sensors.end(),
      [](const SensorActivity &sensor1, const SensorActivity &sensor2) {
        return sensor1.time.time_since_epoch().count() <
               sensor2.time.time_since_epoch().count();
      });
  m_sensors.erase(found);
}

boost::optional<std::string> SensorActivityHandler::getRoomName(
    const wolf::sensor_id &id, const SensorActivityHandler::rooms &rooms_) {
  if (isOutdoor(id)) return std::string("Outdoor");
  for (const auto &room : rooms_)
    for (const auto &sensor : room.sensors)
      if (compareSensors(sensor, id)) return room.name;
  return boost::none;
}

bool SensorActivityHandler::isOutdoor(const wolf::sensor_id &id) {
  return compareSensors(id, m_outdoor.temperature) ||
         compareSensors(id, m_outdoor.humidity);
}

bool SensorActivityHandler::compareSensors(const wolf::sensor_id &id_1,
                                           const wolf::sensor_id &id_2) {
  return id_1 == id_2;
}
