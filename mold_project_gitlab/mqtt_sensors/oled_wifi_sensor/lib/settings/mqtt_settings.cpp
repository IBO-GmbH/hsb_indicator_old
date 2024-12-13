#include "mqtt_settings.hpp"

bool operator==(const mqtt_settings& first, const mqtt_settings& second) {
  return first.client_id == second.client_id &&
         first.mqtt_enabled == second.mqtt_enabled &&
         first.broker_ip == second.broker_ip &&
         first.broker_port == second.broker_port &&
         first.last_will_topic == second.last_will_topic &&
         first.last_will_qos == second.last_will_qos &&
         first.last_will_retain == second.last_will_retain &&
         first.last_will_message == second.last_will_message &&
         first.keep_alive_time == second.keep_alive_time &&
         first.room_name == second.room_name &&
         first.send_interval == second.send_interval;
}

bool operator!=(const mqtt_settings& first, const mqtt_settings& second) {
  return !(first == second);
}

String mqtt_settings::to_string() {
  String settings_string;
  settings_string += ("client_id: " + String(client_id.c_str()));
  settings_string +=
      (String("; mqtt_enabled: ") + (mqtt_enabled ? "true" : "false"));
  if (broker_ip.has_value())
    settings_string += ("; broker_ip: " + String(broker_ip.value().c_str()));
  settings_string += ("; broker_port: " + String(broker_port));
  if (last_will_topic.has_value())
    settings_string +=
        ("; last_will_topic: " + String(last_will_topic.value().c_str()));
  if (last_will_qos.has_value())
    settings_string += ("; last_will_qos: " + String(last_will_qos.value()));
  if (last_will_retain.has_value())
    settings_string += (String("; last_will_retain: ") +
                        (last_will_retain.value() ? "true" : "false"));
  if (last_will_message.has_value())
    settings_string +=
        ("; last_will_message: " + String(last_will_message.value().c_str()));
  if (keep_alive_time.has_value())
    settings_string +=
        ("; keep_alive_time: " + String(keep_alive_time.value()));
  settings_string += ("; room_name: " + String(room_name.c_str()));
  if (send_interval.has_value())
    settings_string += ("; send_interval: " + String(send_interval.value()));
  return settings_string;
}
