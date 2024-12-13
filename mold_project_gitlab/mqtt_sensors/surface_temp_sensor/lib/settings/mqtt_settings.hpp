#ifndef MQTT_SETTINGS_HPP
#define MQTT_SETTINGS_HPP

#pragma once

#include <WString.h>
#include <WiFi.h>

#include <optional>
#include <string>

#define DEFAULT_BROKER_PORT 1883

struct mqtt_settings {
  std::uint8_t mac[6];
  std::uint8_t* return_value = WiFi.macAddress(mac);
  std::uint32_t mac_int = static_cast<std::uint32_t>(mac[5]) |
                          (static_cast<std::uint32_t>(mac[4]) << 8) |
                          (static_cast<std::uint32_t>(mac[3]) << 16);
  std::string client_id = std::string(String(mac_int).c_str());
  bool mqtt_enabled = false;
  std::optional<std::string> broker_ip;
  unsigned int broker_port = DEFAULT_BROKER_PORT;
  std::optional<std::string> last_will_topic;
  std::optional<unsigned int> last_will_qos;
  std::optional<bool> last_will_retain;
  std::optional<std::string> last_will_message;
  std::optional<unsigned int> keep_alive_time;
  std::string room_name = client_id;
  std::optional<unsigned int> averaging_count;

  String to_string();
};

bool operator==(const mqtt_settings& first, const mqtt_settings& second);
bool operator!=(const mqtt_settings& first, const mqtt_settings& second);

#endif