#ifndef MOLD_MQTT_HANDLER_HPP
#define MOLD_MQTT_HANDLER_HPP

#include <boost/asio/io_service.hpp>
#include "log/logger.hpp"
#include "mqtt/include/mosquitto.h"
#include "mqtt/lib/cpp/mosquittopp.h"
#include "mqtt_cache.hpp"
#include "room_creator.hpp"
#include "room_updater.hpp"
#include "wolf/sensor_value_handler.hpp"
#include "wolf/sensor_value_type.hpp"

namespace mold {

class mqtt_handler : public mosqpp::mosquittopp {
 public:
  struct mqtt_settings {
    const std::uint64_t &client_id;
    const std::string &broker_ip;
    const std::uint16_t &broker_port;
  };

  struct mqtt_client {
    std::string client_id;
    wolf::types::uuid_array room_id;
    std::vector<wolf::sensor_id> sensors;
    wolf::sensor_values values;
  };

  mqtt_handler(const mqtt_settings &settings, mqtt_cache &cache,
               room_creator &creator, room_updater &updater,
               boost::asio::io_service &service,
               wolf::sensor_value_handler &value_handler,
               const wolf::room_handler &room_handler);
  ~mqtt_handler() override;
  void remove_client(const wolf::types::uuid_array &room_id);

 private:
  wolf::sensor_value_type parse_topic_to_value_type(
      const std::string &topic) const;
  bool extract_info_from_topic(const std::string &topic, std::string &client_id,
                               std::string &room_name) const;
  void connect_to_broker();
  void subscribe_to_all_topics();
  void handle_start_topic(const std::string &client_id, const bool found_bool);
  void handle_end_topic(const mqtt_client &client,
                        const std::string &room_name) const;
  wolf::sensor_id handle_sensor(const std::string &topic,
                                mqtt_client &client) const;
  void handle_value(const mosquitto_message &message,
                    const wolf::sensor_id &sensor, mqtt_client &client) const;
  void handle_message(const mosquitto_message &message);
  static bool is_start_topic(const std::string &topic);
  static bool is_end_topic(const std::string &topic);
  using mqtt_clients = std::vector<mqtt_client>;
  mqtt_clients::iterator find_client(const std::string &client_id,
                                     bool &found_bool);
  mqtt_clients::iterator find_client(const wolf::types::uuid_array &room_id,
                                     bool &found_bool);

  void on_connect(int rc) override;
  void on_disconnect(int rc) override;
  void on_message(const struct mosquitto_message *message) override;
  void on_subscribe(int /*mid*/, int /*qos_count*/,
                    const int * /*granted_qos*/) override;

 private:
  mutable logging::logger m_logger;
  std::string m_broker_ip;
  std::uint16_t m_broker_port;
  mqtt_cache &m_cache;
  room_creator &m_creator;
  room_updater &m_updater;
  mqtt_clients m_mqtt_clients;
  bool m_running;
  boost::asio::io_service &m_service;
  wolf::sensor_value_handler &m_value_handler;
  const wolf::room_handler &m_room_handler;
};
}  // namespace mold

#endif  // MOLD_MQTT_HANDLER_HPP
