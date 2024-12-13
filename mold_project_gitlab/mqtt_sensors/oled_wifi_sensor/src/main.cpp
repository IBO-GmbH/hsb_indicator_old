#include <LittleFS.h>

#include <stdexcept>

#include "display_handler.hpp"
#include "mqtt_publisher.hpp"
#include "sensor_value_reader.hpp"
#include "settings_handler.hpp"
#include "wifi_connection.hpp"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
#define SENSOR_ADDRESS 0x5E
#define RESET_BUTTON_GPIO 12
#define RESET_BUTTON_TIME 2  // s

sensor_settings current_sensor_settings;
mqtt_settings current_mqtt_settings;

wifi_connection wifi(80);
display_handler display_handler_(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET);
sensor_value_reader value_reader;
mqtt_publisher publisher(current_mqtt_settings);

bool reset_button_pressed = false;
unsigned long reset_check_time = 0;

void set_display_settings() {
  display_handler_.change_rotation_time(
      current_sensor_settings.display_rotation_interval);
  display_handler_.set_display_off_time(
      current_sensor_settings.display_turn_off_time);
  display_handler_.set_units({current_sensor_settings.temperature_unit, "ppm",
                              current_sensor_settings.pressure_unit});
  display_handler_.start_rotation();
}

void set_value_settings() {
  value_reader.change_read_interval(
      current_sensor_settings.sensor_read_interval);
  value_reader.change_sensor_offsets(current_sensor_settings.sensor_offsets);
}

void update_sensor_settings() {
  auto new_sensor_settings = wifi.get_sensor_settings();
  if (new_sensor_settings == current_sensor_settings) return;
  current_sensor_settings = new_sensor_settings;
  set_display_settings();
  set_value_settings();
}

void update_mqtt_settings() {
  auto new_mqtt_settings = wifi.get_mqtt_settings();
  if (new_mqtt_settings == current_mqtt_settings) return;
  current_mqtt_settings = new_mqtt_settings;
  publisher.set_mqtt_settings(current_mqtt_settings);
}

IRAM_ATTR void reset_button_pressed_handler() {
  reset_button_pressed = true;
  reset_check_time = millis();
}

void check_reset() {
  if (!reset_button_pressed) return;
  if (digitalRead(RESET_BUTTON_GPIO)) {
    reset_button_pressed = false;
    return;
  }
  unsigned long now = millis();
  if ((now - reset_check_time) < (RESET_BUTTON_TIME * 1000)) return;
  reset_check_time = now;
  reset_button_pressed = false;
  wifi.reset_wifi_settings();
}

void setup() {
  Serial.begin(115200);

  // allows serving of files from LittleFS
  LittleFS.begin();

  pinMode(RESET_BUTTON_GPIO, INPUT);
  attachInterrupt(digitalPinToInterrupt(RESET_BUTTON_GPIO),
                  reset_button_pressed_handler, FALLING);

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

  wifi.start_wifi_server();

  if (!display_handler_.initialize_display(SCREEN_ADDRESS))
    Serial.println("SSD1306 allocation failed");

  value_reader.sensor_value_init();
  display_handler_.empty_display();

  set_display_settings();
  set_value_settings();
  publisher.set_mqtt_settings(current_mqtt_settings);
}

void loop() {
  try {
    wifi.handle_wifi_connection();
    display_handler_.sensor_values_display_rotation();

    if (wifi.is_connected()) publisher.connect_to_broker();

    if (value_reader.handle_read(SENSOR_ADDRESS)) {
      auto values = value_reader.get_sensor_values();
      display_handler_.set_sensor_values(values);
      publisher.publish_values(values);
    }

    update_sensor_settings();
    update_mqtt_settings();

    check_reset();
  } catch (const std::runtime_error &error) {
    Serial.println(error.what());
  }
}
