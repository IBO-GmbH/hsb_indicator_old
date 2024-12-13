#ifndef WIFI_CONNECTION_HPP
#define WIFI_CONNECTION_HPP

#pragma once

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <PersWiFiManager.h>

#include "mqtt_settings.hpp"
#include "sensor_settings.hpp"

#define DEVICE_NAME "OLED Wifi Sensor"

class wifi_connection {
 public:
  wifi_connection(const int port);
  ~wifi_connection() = default;
  void start_wifi_server();
  void handle_wifi_connection();
  void reset_wifi_settings();
  sensor_settings get_sensor_settings();
  mqtt_settings get_mqtt_settings();
  void set_sensor_settings(const sensor_settings &settings);
  void set_mqtt_settings(const mqtt_settings &settings);
  bool is_connected();

 private:
  bool handleFileRead(String path);
  void read_settings();
  void read_mqtt_settings();
  void read_sensor_settings();

 private:
  ESP8266WebServer wifi_server;
  DNSServer dns_server;
  PersWiFiManager wifi_manager;

  String default_file;

  const char *metaRefreshStr =
      "<script>window.location='/'</script><a href='/'>redirecting...</a>";
  const char *metaErrorStr =
      "<script>window.location='/'</script><a href='/'>MQTT Settings not "
      "found...</a>";

  sensor_settings sensor_settings_;
  mqtt_settings mqtt_settings_;
};

#endif
