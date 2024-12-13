#include "wifi_connection.hpp"

#include <ArduinoJson.h>
// #include <ESP32SSDP.h>
#include <LittleFS.h>

#include <stdexcept>

#include "settings_handler.hpp"

wifi_connection::wifi_connection(const int port)
    : wifi_server(port),
      wifi_manager(wifi_server, dns_server),
      default_file("wifi.htm") {}

void wifi_connection::start_wifi_server() {
  wifi_manager.setApCredentials(DEVICE_NAME);

  wifi_manager.onConnect([this]() { default_file = "mqtt.htm"; });

  wifi_manager.onAp([this]() { default_file = "wifi.htm"; });

  WiFi.persistent(true);
  wifi_manager.setConnectNonBlock(true);
  wifi_manager.begin();

  // serve files from LittleFS
  wifi_server.onNotFound([this]() {
    if (!handleFileRead(wifi_server.uri())) {
      wifi_server.sendHeader("Cache-Control", " max-age=172800");
      wifi_server.send(302, "text/html", metaRefreshStr);
    }
  });  // server.onNotFound

  // handles sent config from webpage
  wifi_server.on("/saveConfig", [this]() {
    read_settings();
    handleFileRead("/");
  });

  // SSDP makes device visible on windows network
  // wifi_server.on("/description.xml", HTTP_GET,
  //                [this]() { SSDP.schema(wifi_server.client()); });
  // SSDP.setSchemaURL("description.xml");
  // SSDP.setHTTPPort(80);
  // SSDP.setName(DEVICE_NAME);
  // SSDP.setURL("/");
  // SSDP.setDeviceType("upnp:rootdevice");
  // SSDP.begin();

  wifi_server.begin();
}

void wifi_connection::handle_wifi_connection() {
  wifi_manager.handleWiFi();

  dns_server.processNextRequest();
  wifi_server.handleClient();
}

void wifi_connection::reset_wifi_settings() {
  wifi_manager.resetSettings();
  wifi_manager.startApMode();
}

sensor_settings wifi_connection::get_sensor_settings() {
  return sensor_settings_;
}

mqtt_settings wifi_connection::get_mqtt_settings() { return mqtt_settings_; }

void wifi_connection::set_sensor_settings(const sensor_settings &settings) {
  if (settings == sensor_settings_) return;
  sensor_settings_ = settings;
}

void wifi_connection::set_mqtt_settings(const mqtt_settings &settings) {
  if (settings == mqtt_settings_) return;
  mqtt_settings_ = settings;
}

bool wifi_connection::is_connected() { return WiFi.status() == WL_CONNECTED; }

std::string wifi_connection::get_ip() {
  return std::string(WiFi.localIP().toString().c_str());
}

bool wifi_connection::handleFileRead(String path) {
  // default file contains parameters
  if (path.endsWith("/")) path += default_file;
  String contentType;
  if (path.endsWith(".htm") || path.endsWith(".html"))
    contentType = "text/html";
  else
    contentType = "text/plain";

  // split filepath and extension
  String prefix = path, ext = "";
  int lastPeriod = path.lastIndexOf('.');
  if (lastPeriod >= 0) {
    prefix = path.substring(0, lastPeriod);
    ext = path.substring(lastPeriod);
  }

  // look for smaller versions of file
  // minified file, good (myscript.min.js)
  if (LittleFS.exists(prefix + ".min" + ext)) path = prefix + ".min" + ext;

  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    if (wifi_server.hasArg("download"))
      wifi_server.sendHeader("Content-Disposition", " attachment;");
    if (wifi_server.uri().indexOf("nocache") < 0)
      wifi_server.sendHeader("Cache-Control", " max-age=172800");

    // optional alt arg (encoded url), server sends redirect to file on the web
    if (WiFi.status() == WL_CONNECTED && wifi_server.hasArg("alt")) {
      wifi_server.sendHeader("Location", wifi_server.arg("alt"), true);
      wifi_server.send(302, "text/plain", "");
    } else {
      // server sends file
      size_t sent = wifi_server.streamFile(file, contentType);
    }
    file.close();
    return true;
  }  // if LittleFS.exists
  return false;
}

void wifi_connection::read_settings() {
  read_sensor_settings();
  read_mqtt_settings();
  if (!settings_handler::save_settings_to_file(mqtt_settings_,
                                               sensor_settings_))
    throw std::runtime_error("Settings could not be written to file!");
}

void wifi_connection::read_mqtt_settings() {
  if (wifi_server.hasArg("mqttEnable"))
    mqtt_settings_.mqtt_enabled = wifi_server.hasArg("mqttEnable");
  if (wifi_server.hasArg("mqttHost"))
    mqtt_settings_.broker_ip = std::string(wifi_server.arg("mqttHost").c_str());
  if (wifi_server.hasArg("mqttPort"))
    mqtt_settings_.broker_port = wifi_server.arg("mqttPort").toInt();
  if (wifi_server.hasArg("willTopic"))
    mqtt_settings_.last_will_topic =
        std::string(wifi_server.arg("willTopic").c_str());
  if (wifi_server.hasArg("willQos"))
    mqtt_settings_.last_will_qos = wifi_server.arg("willQos").toInt();
  if (wifi_server.hasArg("willRetain"))
    mqtt_settings_.last_will_retain = wifi_server.hasArg("willRetain");
  if (wifi_server.hasArg("willMsg"))
    mqtt_settings_.last_will_message =
        std::string(wifi_server.arg("willMsg").c_str());
  if (wifi_server.hasArg("keepAlive"))
    mqtt_settings_.keep_alive_time = wifi_server.arg("keepAlive").toInt();
  if (wifi_server.hasArg("outTopic"))
    mqtt_settings_.room_name = std::string(wifi_server.arg("outTopic").c_str());
  if (wifi_server.hasArg("averagingCount"))
    mqtt_settings_.averaging_count =
        wifi_server.arg("averagingCount").toInt();
}

void wifi_connection::read_sensor_settings() {
  if (wifi_server.hasArg("turnOffTime"))
    sensor_settings_.display_turn_off_time =
        wifi_server.arg("turnOffTime").toInt();
  if (wifi_server.hasArg("readInterval"))
    sensor_settings_.sensor_read_interval =
        wifi_server.arg("readInterval").toInt();
  if (wifi_server.hasArg("tempUnit"))
    sensor_settings_.temperature_unit =
        std::string(wifi_server.arg("tempUnit").c_str());
  if (wifi_server.hasArg("tempOffset"))
    sensor_settings_.sensor_offset = wifi_server.arg("tempOffset").toDouble();
}