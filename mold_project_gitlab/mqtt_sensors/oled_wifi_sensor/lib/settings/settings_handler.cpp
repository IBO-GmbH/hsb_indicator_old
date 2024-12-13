#include "settings_handler.hpp"

#include <LittleFS.h>

settings_handler::settings_handler() {}

bool settings_handler::save_settings_to_file(
    const mqtt_settings& mqtt_settings_,
    const sensor_settings& sensor_settings_) {
  File settings_file = LittleFS.open(settings_file_name, "w");
  if (!settings_file) return false;
  save_mqtt_settings(settings_file, mqtt_settings_);
  save_sensor_settings(settings_file, sensor_settings_);
  settings_file.close();
  return true;
}

bool settings_handler::read_settings_from_file(
    mqtt_settings& mqtt_settings_, sensor_settings& sensor_settings_) {
  File settings_file = LittleFS.open(settings_file_name, "r");
  if (!settings_file) return false;
  read_settings(settings_file, mqtt_settings_, sensor_settings_);
  settings_file.close();
  return true;
}

bool settings_handler::settings_file_exists() {
  return LittleFS.exists(settings_file_name);
}

void settings_handler::save_mqtt_settings(File& settings_file,
                                          const mqtt_settings& settings) {
  settings_file.print(
      "mqttEnable=" + String(static_cast<int>(settings.mqtt_enabled)) + ";");
  if (settings.broker_ip.has_value())
    settings_file.print(
        "mqttHost=" + String(settings.broker_ip.value().c_str()) + ";");
  settings_file.print("mqttPort=" + String(settings.broker_port) + ";");
  if (settings.last_will_topic.has_value())
    settings_file.print(
        "willTopic=" + String(settings.last_will_topic.value().c_str()) + ";");
  if (settings.last_will_qos.has_value())
    settings_file.print("willQos=" + String(settings.last_will_qos.value()) +
                        ";");
  if (settings.last_will_retain.has_value())
    settings_file.print(
        "willRetain=" +
        String(static_cast<int>(settings.last_will_retain.value())) + ";");
  if (settings.last_will_message.has_value())
    settings_file.print(
        "willMsg=" + String(settings.last_will_message.value().c_str()) + ";");
  if (settings.keep_alive_time.has_value())
    settings_file.print(
        "keepAlive=" + String(settings.keep_alive_time.value()) + ";");
  settings_file.print("outTopic=" + String(settings.room_name.c_str()) + ";");
  if (settings.send_interval.has_value())
    settings_file.print(
        "outPublishInterval=" + String(settings.send_interval.value()) + ";");
}

void settings_handler::save_sensor_settings(File& settings_file,
                                            const sensor_settings& settings) {
  settings_file.print("proximityEnable=" +
                      String(static_cast<int>(settings.proximity_enabled)) +
                      ";");
  settings_file.print("turnOffTime=" + String(settings.display_turn_off_time) +
                      ";");
  settings_file.print(
      "displayInterval=" + String(settings.display_rotation_interval) + ";");
  settings_file.print("readInterval=" + String(settings.sensor_read_interval) +
                      ";");
  settings_file.print("tempUnit=" + String(settings.temperature_unit.c_str()) +
                      ";");
  settings_file.print("pressUnit=" + String(settings.pressure_unit.c_str()) +
                      ";");
  settings_file.print("tempOffset=" + String(settings.sensor_offsets[0]) + ";");
  settings_file.print("co2Offset=" + String(settings.sensor_offsets[1]) + ";");
  settings_file.print("pressOffset=" + String(settings.sensor_offsets[2]) +
                      ";");
}

void settings_handler::read_settings(File& settings_file,
                                     mqtt_settings& mqtt_settings_,
                                     sensor_settings& sensor_settings_) {
  String setting_read;
  while (true) {
    setting_read = settings_file.readStringUntil(';');
    if (setting_read.isEmpty()) break;
    auto equal_sign = setting_read.indexOf("=");
    String setting_name = setting_read.substring(0, equal_sign);
    String setting_value = setting_read.substring(equal_sign + 1);
    parse_mqtt_settings(mqtt_settings_, setting_name, setting_value);
    parse_sensor_settings(sensor_settings_, setting_name, setting_value);
  }
}

void settings_handler::parse_mqtt_settings(mqtt_settings& settings,
                                           const String& setting_name,
                                           const String& setting_value) {
  if (setting_name.equals("mqttEnable"))
    settings.mqtt_enabled = static_cast<bool>(std::stoi(setting_value.c_str()));
  else if (setting_name.equals("mqttHost"))
    settings.broker_ip = std::string(setting_value.c_str());
  else if (setting_name.equals("mqttPort"))
    settings.broker_port = std::stoi(setting_value.c_str());
  else if (setting_name.equals("willTopic"))
    settings.last_will_topic = std::string(setting_value.c_str());
  else if (setting_name.equals("willQos"))
    settings.last_will_qos = std::stoi(setting_value.c_str());
  else if (setting_name.equals("willRetain"))
    settings.last_will_retain =
        static_cast<bool>(std::stoi(setting_value.c_str()));
  else if (setting_name.equals("willMsg"))
    settings.last_will_message = std::string(setting_value.c_str());
  else if (setting_name.equals("keepAlive"))
    settings.keep_alive_time = std::stoi(setting_value.c_str());
  else if (setting_name.equals("outTopic"))
    settings.room_name = std::string(setting_value.c_str());
  else if (setting_name.equals("outPublishInterval"))
    settings.send_interval = std::stoi(setting_value.c_str());
}

void settings_handler::parse_sensor_settings(sensor_settings& settings,
                                             const String& setting_name,
                                             const String& setting_value) {
  if (setting_name.equals("proximityEnable"))
    settings.proximity_enabled =
        static_cast<bool>(std::stoi(setting_value.c_str()));
  else if (setting_name.equals("turnOffTime"))
    settings.display_turn_off_time = std::stoi(setting_value.c_str());
  else if (setting_name.equals("displayInterval"))
    settings.display_rotation_interval = std::stoi(setting_value.c_str());
  else if (setting_name.equals("readInterval"))
    settings.sensor_read_interval = std::stoi(setting_value.c_str());
  else if (setting_name.equals("tempUnit"))
    settings.temperature_unit = std::string(setting_value.c_str());
  else if (setting_name.equals("pressUnit"))
    settings.pressure_unit = std::string(setting_value.c_str());
  else if (setting_name.equals("tempOffset"))
    settings.sensor_offsets[0] = std::stod(setting_value.c_str());
  else if (setting_name.equals("co2Offset"))
    settings.sensor_offsets[1] = std::stod(setting_value.c_str());
  else if (setting_name.equals("pressOffset"))
    settings.sensor_offsets[2] = std::stod(setting_value.c_str());
}
