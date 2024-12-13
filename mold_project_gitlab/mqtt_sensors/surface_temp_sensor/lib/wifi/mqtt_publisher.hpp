#ifndef MQTT_PUBLISHER_HPP
#define MQTT_PUBLISHER_HPP

#pragma once

#include <WiFi.h>
#include <PubSubClient.h>

#include "mqtt_settings.hpp"

class mqtt_publisher {
 public:
  mqtt_publisher(const mqtt_settings& settings_);
  ~mqtt_publisher() = default;
  void set_mqtt_settings(const mqtt_settings& new_settings);
  void connect_to_broker();
  void publish_value(const double& value);
  bool connected_to_broker();

 private:
  bool connect_with_settings();

 private:
  WiFiClient wifi_client;
  PubSubClient client;
  mqtt_settings settings;
};

#endif