#ifndef MQTT_SETTINGS_HPP
#define MQTT_SETTINGS_HPP

#pragma once

#include <Arduino.h>
#include <WString.h>

#include <optional>
#include <string>

#define DEFAULT_BROKER_PORT 1883

struct mqtt_settings {
  std::string client_id = std::to_string(ESP.getChipId());
  bool mqtt_enabled = false;
  std::optional<std::string> broker_ip;
  unsigned int broker_port = DEFAULT_BROKER_PORT;
  std::optional<std::string> last_will_topic;
  std::optional<unsigned int> last_will_qos;
  std::optional<bool> last_will_retain;
  std::optional<std::string> last_will_message;
  std::optional<unsigned int> keep_alive_time;
  std::string room_name = client_id;
  std::optional<unsigned int> send_interval;

  String to_string();
};

bool operator==(const mqtt_settings& first, const mqtt_settings& second);
bool operator!=(const mqtt_settings& first, const mqtt_settings& second);

#endif