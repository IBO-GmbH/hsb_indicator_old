#include <LittleFS.h>
#include <M5StickCPlus.h>
#include <SPI.h>
#include <esp_bt.h>
#include <esp_wifi.h>

#include <functional>
#include <numeric>
#include <stdexcept>

#include "button_handler.hpp"
#include "display_handler.hpp"
#include "driver/adc.h"
#include "mqtt_publisher.hpp"
#include "sensor_value_reader.hpp"
#include "settings_handler.hpp"
#include "wifi_connection.hpp"

#define RESET_BUTTON_GPIO 37
#define RESET_TIME 5                // s
#define MULTI_PRESS_DELAY_TIME 300  // ms
#define MULTI_PRESS_NUMBER 3
#define I2C_ADDRESS 0x48
#define uS_TO_S_FACTOR \
  1000000ULL /* Conversion factor for micro seconds to seconds */
#define CONNECT_TIME_OUT 20  // s

RTC_DATA_ATTR double value = 0.0;
RTC_DATA_ATTR double read_values[100];
RTC_DATA_ATTR unsigned int read_count = 0;

RTC_DATA_ATTR sensor_settings current_sensor_settings;
RTC_DATA_ATTR mqtt_settings current_mqtt_settings;

bool config_mode = false;

wifi_connection wifi(80);
display_handler display_handler_;
button_handler button_handler_(M5.BtnA, display_handler_, wifi,
                               MULTI_PRESS_NUMBER, MULTI_PRESS_DELAY_TIME,
                               RESET_TIME);
sensor_value_reader value_reader(I2C_ADDRESS);
mqtt_publisher publisher(current_mqtt_settings);

void update_sensor_settings() {
  auto new_sensor_settings = wifi.get_sensor_settings();
  if (new_sensor_settings == current_sensor_settings) return;
  current_sensor_settings = new_sensor_settings;
}

void update_mqtt_settings() {
  auto new_mqtt_settings = wifi.get_mqtt_settings();
  if (new_mqtt_settings == current_mqtt_settings) return;
  current_mqtt_settings = new_mqtt_settings;
  publisher.set_mqtt_settings(current_mqtt_settings);
}

void handle_settings_start() {
  // allows serving of files from LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS!");
    return;
  }

  if (!settings_handler::settings_file_exists()) {
    if (!settings_handler::save_settings_to_file(current_mqtt_settings,
                                                 current_sensor_settings))
      Serial.println("Settings could not be written to file!");
  } else {
    if (!settings_handler::read_settings_from_file(current_mqtt_settings,
                                                   current_sensor_settings))
      Serial.println("Settings could not be read from file!");
    else {
      wifi.set_sensor_settings(current_sensor_settings);
      wifi.set_mqtt_settings(current_mqtt_settings);
    }
  }
}

void go_to_sleep() {
  display_handler_.empty_display();
  M5.Axp.ScreenBreath(0);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();

  adc_power_off();
  esp_wifi_stop();
  esp_bt_controller_disable();

  esp_deep_sleep_start();
}

bool wait_till_timeout(const std::string &error_text,
                       std::function<bool()> condition) {
  auto check = millis();
  while (!condition()) {
    if ((millis() - check) >= CONNECT_TIME_OUT * 1000) {
      Serial.println(error_text.c_str());
      return false;
    }
  }
  return true;
}

bool connect_to_wifi() {
  wifi.start_wifi_server();

  wifi.handle_wifi_connection();

  display_handler_.display_string(std::string("Connecting to Wifi..."));

  if (!wait_till_timeout(std::string("Could not connect to Wifi!"),
                         [&]() { return wifi.is_connected(); })) {
    display_handler_.display_string(std::string("Failed!"));
    delay(500);
    return false;
  }
  display_handler_.display_string(std::string("Connected!"));
  return true;
}

void handle_send(const double value_to_send) {
  if (!connect_to_wifi()) return;

  publisher.set_mqtt_settings(current_mqtt_settings);
  publisher.connect_to_broker();

  if (!wait_till_timeout(std::string("Could not connect to Broker!"),
                         [&]() { return publisher.connected_to_broker(); }))
    return;

  publisher.publish_value(value_to_send);
}

void handle_read_wake_up() {
  if (!value_reader.sensor_value_init()) {
    Serial.println("An Error has occurred while starting I2C!");
    return;
  }

  value = value_reader.handle_read(current_sensor_settings.sensor_offset);
  if(value >= 255.f) return;
  read_values[read_count] = value;
  ++read_count;
  if (read_count < current_mqtt_settings.averaging_count) return;
  const double averaged_value =
      std::accumulate(read_values, read_values + read_count, 0.0) /
      static_cast<double>(read_count);
  read_count = 0;
  handle_send(averaged_value);
}

void handle_single_press() {
  if (config_mode) return;
  display_handler_.display_sensor_value(
      value, current_sensor_settings.temperature_unit);
}

void handle_long_press() { wifi.reset_wifi_settings(); }

void handle_multi_press() {
  config_mode = !config_mode;
  if (config_mode) {
    connect_to_wifi();
    display_handler_.display_ip_address(wifi.get_ip());
  } else {
    update_sensor_settings();
    update_mqtt_settings();
    display_handler_.display_sensor_value(
        value, current_sensor_settings.temperature_unit);
  }
}

void handle_button_wake_up() {
  display_handler_.init_display();
  display_handler_.display_sensor_value(
      value, current_sensor_settings.temperature_unit);
  while (!display_handler_.check_if_off(
      current_sensor_settings.display_turn_off_time, config_mode)) {
    if (config_mode) wifi.handle_wifi_connection();
    M5.update();
    const auto state = button_handler_.check_pressed_state(
        value, current_sensor_settings.temperature_unit);
    switch (state) {
      case button_handler::pressed_state::single_press:
        handle_single_press();
        break;
      case button_handler::pressed_state::long_press:
        handle_long_press();
        break;
      case button_handler::pressed_state::multi_press:
        handle_multi_press();
        break;
    }
  }
}

void handle_wake_up() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      handle_button_wake_up();
      break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:  // first boot
      display_handler_.init_display();
    case ESP_SLEEP_WAKEUP_TIMER:
      handle_read_wake_up();
      break;
    default:
      Serial.printf(
          "\nWakeup was not caused by power on, timer or button: %d\n",
          wakeup_reason);
      break;
  }
}

void setup() {
  setCpuFrequencyMhz(80);
  M5.begin();
  M5.Axp.ScreenBreath(0);

  Serial.begin(115200);

  handle_settings_start();

  try {
    handle_wake_up();
  } catch (const std::runtime_error &error) {
    Serial.println(error.what());
    display_handler_.init_display();
    display_handler_.display_string(std::string(error.what()));
    delay(1000);
  }

  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(BUTTON_A_PIN), LOW);
  esp_sleep_enable_timer_wakeup(current_sensor_settings.sensor_read_interval *
                                uS_TO_S_FACTOR * 60);

  go_to_sleep();
}

void loop() {}
