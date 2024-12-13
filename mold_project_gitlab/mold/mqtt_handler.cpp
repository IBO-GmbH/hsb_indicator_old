#include "mqtt_handler.hpp"

#include "sensor_id_mqtt.hpp"
#include "wolf/uuid_generator.hpp"

mold::mqtt_handler::mqtt_handler(const mqtt_settings &settings,
                                 mqtt_cache &cache, mold::room_creator &creator,
                                 mold::room_updater &updater,
                                 boost::asio::io_service &service,
                                 wolf::sensor_value_handler &value_handler,
                                 const wolf::room_handler &room_handler)
    : mosquittopp(std::to_string(settings.client_id).c_str()),
      m_logger("mqtt_handler"),
      m_broker_ip(settings.broker_ip),
      m_broker_port(settings.broker_port),
      m_cache(cache),
      m_creator(creator),
      m_updater(updater),
      m_running(true),
      m_service(service),
      m_value_handler(value_handler),
      m_room_handler(room_handler) {
  const auto init_return = mosqpp::lib_init();
  if (init_return != MOSQ_ERR_SUCCESS) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "could not initialize mosquitto lib, error: "
        << mosqpp::strerror(init_return);
    return;
  }
  const auto loaded_clients = m_cache.get_all();
  for (const auto &client : loaded_clients)
    m_mqtt_clients.push_back({client.client_id, client.room_id, {}, {}});
  loop_start();
  connect_to_broker();
}

mold::mqtt_handler::~mqtt_handler() {
  m_running = false;
  disconnect();
  loop_stop();
  mosqpp::lib_cleanup();
}

void mold::mqtt_handler::remove_client(const wolf::types::uuid_array &room_id) {
  bool found = false;
  const auto found_client = find_client(room_id, found);
  if (!found) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "no client found for room_id, room wasn't from mqtt sensor, id:"
        << room_id;
    return;
  }
  m_mqtt_clients.erase(found_client);
  m_cache.remove(found_client->client_id);
}

wolf::sensor_value_type mold::mqtt_handler::parse_topic_to_value_type(
    const std::string &topic) const {
  if (topic.find("temperature") != std::string::npos)
    return wolf::sensor_value_type::temperature;
  if (topic.find("co2") != std::string::npos)
    return wolf::sensor_value_type::co2;
  if (topic.find("air_pressure") != std::string::npos)
    return wolf::sensor_value_type::air_pressure;
  LOGGING_LOG(m_logger, logging::severity::warning)
      << "could not find matching sensor_value_type for topic: " << topic;
  return wolf::sensor_value_type::unknown;
}

bool mold::mqtt_handler::extract_info_from_topic(const std::string &topic,
                                                 std::string &client_id,
                                                 std::string &room_name) const {
  char **tokens;
  int token_count = 0;
  const auto split_return =
      mosqpp::sub_topic_tokenise(topic.c_str(), &tokens, &token_count);
  if (split_return != MOSQ_ERR_SUCCESS) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not split topic string, error: "
        << mosqpp::strerror(split_return);
    return false;
  }
  if (token_count != 3) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "topic string was not split in enough tokens, token_count: "
        << token_count;
    return false;
  }
  client_id = std::string(tokens[0]);
  room_name = std::string(tokens[1]);
  std::replace(room_name.begin(), room_name.end(), '_', ' ');
  return true;
}

void mold::mqtt_handler::connect_to_broker() {
  const auto connect_return = connect_async(m_broker_ip.c_str(), m_broker_port);
  if (connect_return != MOSQ_ERR_SUCCESS)
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not connect to broker, error: "
        << mosqpp::strerror(connect_return);
  // reconnect if not able to connect
}

void mold::mqtt_handler::subscribe_to_all_topics() {
  const auto subscription_return = subscribe(nullptr, "#");
  if (subscription_return != MOSQ_ERR_SUCCESS) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not subscribe, is there a valid connection? error: "
        << mosqpp::strerror(subscription_return);
    connect_to_broker();
  }
}

void mold::mqtt_handler::handle_start_topic(const std::string &client_id,
                                            const bool found_bool) {
  if (found_bool) return;
  const mqtt_client new_client{
      client_id, wolf::uuid_generator::generate_as_array(), {}, {}};
  m_mqtt_clients.push_back(new_client);
  m_cache.add({new_client.client_id, new_client.room_id});
}

void mold::mqtt_handler::handle_end_topic(const mqtt_client &client,
                                          const std::string &room_name) const {
  m_service.post([this, room_name, client]() {
    try {
      auto room_of_client = m_room_handler.get(client.room_id);
      std::unique_copy(client.sensors.cbegin(), client.sensors.cend(),
                       room_of_client.sensors.begin());
      const bool has_config = m_updater.update_room_and_config(room_of_client);
      if (!has_config)
        LOGGING_LOG(m_logger, logging::severity::warning)
            << "room does not have config although it exists";
    } catch (const std::exception &) {
      LOGGING_LOG(m_logger, logging::severity::verbose)
          << "could not get room, wasn't created yet";
      m_creator.create(client.room_id, room_name, client.sensors,
                       wolf::types::uuid_array{}, false);
    }
    m_value_handler.handle_list(client.values);
  });
}

wolf::sensor_id mold::mqtt_handler::handle_sensor(const std::string &topic,
                                                  mqtt_client &client) const {
  const auto sensor_type = parse_topic_to_value_type(topic);
  const auto sensor = sensor_id_mqtt::create(client.client_id, sensor_type);
  const auto found_sensor =
      std::find(client.sensors.cbegin(), client.sensors.cend(), sensor);
  if (found_sensor == client.sensors.cend()) client.sensors.push_back(sensor);
  return sensor;
}

void mold::mqtt_handler::handle_value(const mosquitto_message &message,
                                      const wolf::sensor_id &sensor,
                                      mqtt_client &client) const {
  const auto value = std::stof(static_cast<char *>(message.payload));
  const auto now = std::chrono::system_clock::now();
  const auto new_sensor_value = wolf::sensor_value{sensor, value, now};
  const auto found_value = std::find_if(
      client.values.begin(), client.values.end(),
      [sensor](const wolf::sensor_value &value) { return value.id == sensor; });
  if (found_value == client.values.end()) {
    client.values.push_back(new_sensor_value);
    return;
  }
  *found_value = new_sensor_value;
}

void mold::mqtt_handler::handle_message(const mosquitto_message &message) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "received message: " << message.topic << ", "
      << static_cast<char *>(message.payload);
  auto topic = std::string(message.topic);
  std::string client_id;
  std::string room_name;
  if (!extract_info_from_topic(topic, client_id, room_name)) return;
  bool found_bool = true;
  auto found = find_client(client_id, found_bool);
  if (is_start_topic(topic)) {
    handle_start_topic(client_id, found_bool);
    return;
  }
  if (!found_bool) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "client not yet linked to room but first message was not start, "
           "transmission is skipped!";
    return;
  }
  if (is_end_topic(topic)) {
    handle_end_topic(*found, room_name);
    return;
  }
  const auto sensor = handle_sensor(topic, *found);
  handle_value(message, sensor, *found);
}

bool mold::mqtt_handler::is_start_topic(const std::string &topic) {
  return (topic.find("start") != std::string::npos);
}

bool mold::mqtt_handler::is_end_topic(const std::string &topic) {
  return (topic.find("end") != std::string::npos);
}

mold::mqtt_handler::mqtt_clients::iterator mold::mqtt_handler::find_client(
    const std::string &client_id, bool &found_bool) {
  const auto found = std::find_if(m_mqtt_clients.begin(), m_mqtt_clients.end(),
                                  [&client_id](const mqtt_client &client) {
                                    return client.client_id == client_id;
                                  });
  if (found == m_mqtt_clients.end()) found_bool = false;
  return found;
}

mold::mqtt_handler::mqtt_clients::iterator mold::mqtt_handler::find_client(
    const wolf::types::uuid_array &room_id, bool &found_bool) {
  const auto found = std::find_if(m_mqtt_clients.begin(), m_mqtt_clients.end(),
                                  [&room_id](const mqtt_client &client) {
                                    return client.room_id == room_id;
                                  });
  if (found == m_mqtt_clients.end()) found_bool = false;
  return found;
}

void mold::mqtt_handler::on_connect(int rc) {
  if (rc != 0) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not successfully connect to broker, rc: " << rc;
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "successfully connected to broker!";
  subscribe_to_all_topics();
}

void mold::mqtt_handler::on_disconnect(int rc) {
  if (!m_running) return;
  LOGGING_LOG(m_logger, logging::severity::warning)
      << "disconnected from broker, rc: " << rc;
  connect_to_broker();
}

void mold::mqtt_handler::on_message(const mosquitto_message *message) {
  handle_message(*message);
}

void mold::mqtt_handler::on_subscribe(int, int, const int *) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "successfully subscribed to all topics!";
}
