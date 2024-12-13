#ifndef SETTINGS_HANDLER_HPP
#define SETTINGS_HANDLER_HPP

#pragma once

#include <FS.h>

#include <mqtt_settings.hpp>
#include <sensor_settings.hpp>

const String settings_file_name("/settings.txt");

class settings_handler {
 public:
  settings_handler();
  ~settings_handler() = default;

  static bool save_settings_to_file(const mqtt_settings& mqtt_settings_,
                                    const sensor_settings& sensor_settings_);
  static bool read_settings_from_file(mqtt_settings& mqtt_settings_,
                                      sensor_settings& sensor_settings_);
  static bool settings_file_exists();

 private:
  static void save_mqtt_settings(File& settings_file,
                                 const mqtt_settings& settings);
  static void save_sensor_settings(File& settings_file,
                                   const sensor_settings& settings);
  static void read_settings(File& settings_file, mqtt_settings& mqtt_settings_,
                            sensor_settings& sensor_settings_);
  static void parse_mqtt_settings(mqtt_settings& settings, const String& setting_name, const String& setting_value);
  static void parse_sensor_settings(sensor_settings& settings, const String& setting_name, const String& setting_value);
};

#endif