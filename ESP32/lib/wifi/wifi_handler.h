#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#pragma once

#include <cstring>
#include <span>
#include <string>

// #include "freertos/FreeRTOS.h"
// #include "freertos/event_groups.h"
//
// #include "esp_event.h"
#include "esp_log.h"
// #include "esp_system.h"
#include "esp_wifi.h"
// #include "nvs_flash.h"
// #include "regex.h"
// #include "esp_smartconfig.h"

#define DEFAULT_SCAN_LIST_SIZE 10

#define EXAMPLE_ESP_MAXIMUM_RETRY 5

// Scan only non overlapping channels using Channel bitmap
#define USE_SCAN_CHANNEL_BITMAP false

class Wifi_handler {
 public:
  Wifi_handler();
  ~Wifi_handler();

  void event_handler(void *event_handler_arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data);

  // initializes Wifi in Station Mode
  esp_err_t init_sta();
  esp_err_t deinit();

  esp_err_t connect_to_wifi();
  esp_err_t connect_to_wifi(std::string_view ssid, std::string_view password);
  esp_err_t set_ssid_and_pw(std::string_view ssid, std::string_view password);

  // returns array or vector of scanned AP's up to the size of the array
  esp_err_t scan(std::span<wifi_ap_record_t> ap_info);
  // prints scan data to the monitor
  void log_ap_info(std::span<wifi_ap_record_t> ap_info);
  // check if ssid is contained in wifi records and returns true if found
  bool ssid_in_records(std::span<wifi_ap_record_t> ap_info,
                       std::string_view ssid);

#ifdef USE_CHANNEL_BTIMAP
  static void array_2_channel_bitmap(const uint8_t channel_list[],
                                     const uint8_t channel_list_size,
                                     wifi_scan_config_t *scan_config);
#endif  // USE_CHANNEL_BTIMAP

 private:
  // this function is used that a class member function can be used as event
  // handler function
  static void event_handler_static(void *event_handler_arg,
                                   esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);

  const std::string tag;
  std::string m_ssid;
  std::string m_password;
  uint8_t m_retry_num = 0;
  bool m_connecting = false;
  esp_netif_t *wifi_netif = NULL;
};

#endif  // WIFI_HANDLER_H