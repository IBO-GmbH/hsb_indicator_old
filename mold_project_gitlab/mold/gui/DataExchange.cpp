#include "DataExchange.hpp"

#include "PostToGUIThread.hpp"
#include "WifiHandler.hpp"
#include "mold/configuration_state_handler.hpp"
#include "mold/options_handler.hpp"
#include "mold/tone_enabled_handler.hpp"
#include "wolf/enocean_handler.hpp"
#include "wolf/esp3_parser.hpp"
#include "wolf/login_handler.hpp"
#include "wolf/network/foreign_sensor/requester.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/room_handler.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_value.hpp"
#include "wolf/sensor_value_type_helper.hpp"
#include "wolf/thread_checker.hpp"

using namespace logging;
using namespace mold;

DataExchange::DataExchange(const WolfServices &services)
    : m_logger("DataExchange"), m_services(services) {
  wolf::thread_checker::throw_if_not_service("DataExchange::DataExchange");
}

void DataExchange::initialise() {
  LOGGING_LOG(m_logger, severity::normal) << "initalising DataExchange";

  if (m_services.configurations) {
    m_configurations = m_services.configurations->get_all();
    m_outdoor_sensor_id = m_services.configurations->get_outdoor_sensor();

    m_services.configurations->signal_added.connect(
        [this](const mold::configuration &config) {
          postToGUIThread(
              [this, config] { handle_configuration_added(config); });
        });
    m_services.configurations->signal_updated.connect(
        [this](const mold::configuration &config) {
          postToGUIThread(
              [this, config] { handle_configuration_updated(config); });
        });
    m_services.configurations->signal_removed.connect(
        [this](const wolf::types::uuid_array &id) {
          postToGUIThread([this, id] { handle_configuration_removed(id); });
        });
    m_services.configurations->signal_outdoor_changed.connect(
        [this](const auto &, const wolf::outdoor_sensor &sensor_id) {
          postToGUIThread([this, sensor_id] {
            m_outdoor_sensor_id = sensor_id;
            configuration_outdoor_sensor_changed(sensor_id);
          });
        });
  }

  if (m_services.rooms) {
    m_rooms = m_services.rooms->get_all();

    m_services.rooms->signal_added.connect([this](const wolf::room &added) {
      postToGUIThread([this, added] {
        LOGGING_LOG(m_logger, severity::verbose) << "adding room " << added;
        m_rooms.push_back(added);
        m_services.service->post(
            [power_calculator = m_services.power_calculator_, added]() {
              power_calculator->handle_room_added(added);
            });
        emit room_added(added);
      });
    });
    m_services.rooms->signal_updated.connect([this](const wolf::room &room_) {
      postToGUIThread([this, room_] { handle_room_updated(room_); });
    });
    m_services.rooms->signal_removed.connect(
        [this](const wolf::types::uuid_array &id) {
          postToGUIThread([this, id] { handle_room_removed(id); });
        });
  }

  if (m_services.learn_handler) {
    m_profiles = m_services.learn_handler->get_all();

    m_services.learn_handler->signal_update.connect(
        [this](const wolf::sensor_profile &) {
          const auto profiles = m_services.learn_handler->get_all();
          postToGUIThread([this, profiles] {
            m_profiles = profiles;
            emit profiles_updated();
          });
        });
  }

  if (m_services.options_handler) {
    m_frsi = m_services.options_handler->get_frsi();

    m_services.options_handler->signal_frsi_updated.connect([this]() {
      const float value = m_services.options_handler->get_frsi();
      postToGUIThread([this, value] { handle_frsi_changed(value); });
    });

    m_rotation_enabled = m_services.options_handler->get_rotation_enabled();

    m_services.options_handler->signal_rotation_enabled_changed.connect(
        [this](const bool enabled) {
          postToGUIThread([this, enabled] { m_rotation_enabled = enabled; });
        });
  }

  if (m_services.configuration_values) {
    m_outdoor_values =
        m_services.configuration_values->get_last_outdoor_value();
    m_config_values = m_services.configuration_values->get_all();

    m_services.configuration_values->signal_values_outdoor.connect(
        [this](const configuration_values_handler::outdoor_values &value) {
          postToGUIThread([this, value] {
            m_outdoor_values = value;
            emit configuration_values_changed();
          });
        });
    m_services.configuration_values->signal_value.connect(
        [this](const configuration_values &) {
          const auto config_values = m_services.configuration_values->get_all();
          postToGUIThread([this, config_values] {
            m_config_values = config_values;
            emit configuration_values_changed();
          });
        });
  }

  if (m_services.configuration_states) {
    m_config_states = m_services.configuration_states->get_all();

    m_services.configuration_states->signal_state_changed.connect(
        [this](const wolf::types::uuid_array &, const calculation_state &) {
          const auto config_states = m_services.configuration_states->get_all();
          postToGUIThread([this, config_states] {
            m_config_states = config_states;
            emit configuration_states_changed();
          });
        });
    m_services.configuration_states->signal_added.connect(
        [this](const wolf::types::uuid_array &, const configuration_state &) {
          const auto config_states = m_services.configuration_states->get_all();
          postToGUIThread(
              [this, config_states] { m_config_states = config_states; });
        });
  }

  m_config_mold = m_services.mold_value_handler_->get_all();
  m_services.mold_value_handler_->signal_value.connect([this](const auto &) {
    const auto config_mold = m_services.mold_value_handler_->get_all();
    postToGUIThread([this, config_mold] {
      m_config_mold = config_mold;
      emit configuration_molds_changed();
    });
  });

  if (m_services.value_handler) {
    m_values = m_services.value_handler->get_all();
    m_services.value_handler->signal_values.connect(
        [this](const wolf::sensor_values &values) { handle_values(values); });
  }

  if (m_services.esp3_parser) {
    m_services.esp3_parser->signal_learned.connect(
        [this](const wolf::types::id_esp3 &id, const std::uint8_t &org,
               const std::uint8_t &func, const std::uint8_t &type) {
          postToGUIThread([this, id, org, func, type] {
            emit learned(id, org, func, type);
          });
        });

    m_services.esp3_parser->signal_data_4bs.connect(
        [this](const wolf::types::id_esp3 &id, const wolf::types::data &data,
               const int rssi) {
          postToGUIThread(
              [this, id, data, rssi] { emit data_4bs_or_vld(id, data, rssi); });
        });
    m_services.esp3_parser->signal_data_vld.connect(
        [this](const wolf::types::id_esp3 &id, const wolf::types::data &data,
               const int rssi) {
          postToGUIThread(
              [this, id, data, rssi] { emit data_4bs_or_vld(id, data, rssi); });
        });
  }
  m_services.enocean_handler->signal_values.connect(
      [this](const auto &values, const int rssi) {
        assert(!values.empty());
        std::vector<wolf::sensor_value> activity_values = values;
        const auto new_end = std::remove_if(
            activity_values.begin(), activity_values.end(),
            [](const wolf::sensor_value &value) {
              return value.id.get_type() == wolf::sensor_value_type::unknown;
            });
        activity_values.erase(new_end, activity_values.end());
        if (activity_values.empty()) return;
        postToGUIThread([this, value = activity_values.front(), rssi] {
          emit enocean_values_read(value.id, rssi, m_rooms);
        });
      });

  if (m_services.tone_enabled_handler) {
    m_tone_enabled = m_services.tone_enabled_handler->is_enabled();

    m_services.tone_enabled_handler->signal_changed.connect(
        [this](bool enabled) {
          postToGUIThread([this, enabled] { m_tone_enabled = enabled; });
        });
  }

  if (m_services.controller_handler) {
    m_controller_values = m_services.controller_handler->get_all_values();

    m_services.controller_handler->signal_update_gui.connect(
        [this](const values_list &values) {
          postToGUIThread([this, values] {
            m_controller_values = values;
            emit controller_updated();
          });
        });
  }

  if (m_services.display_handler) {
    m_services.display_handler->signal_on.connect([this](const bool on) {
      postToGUIThread([this, on] { emit display_on_changed(on); });
    });
  }

  if (m_services.nightmode_handler) {
    m_nightmode_enabled = m_services.nightmode_handler->is_enabled();

    m_services.nightmode_handler->signal_enabled.connect(
        [this](const bool enabled) {
          postToGUIThread([this, enabled] { m_nightmode_enabled = enabled; });
        });
  }

  initialise_wifi();

  LOGGING_LOG(m_logger, severity::normal) << "initalising DataExchange done";
}

void DataExchange::initialise_wifi() {
  bool is_initialised = WifiHandler::initialise(
      m_wifi_scanner, m_wifi_rescanner, m_wifi_state_watcher, m_active_getter);
  if (!is_initialised) {
    LOGGING_LOG(m_logger, logging::severity::normal)
        << "could not find a wifi device";
    return;
  }
  m_access_points = m_wifi_scanner->get_access_points();

  m_wifi_scanner->signal_added.connect([this](access_point added) {
    m_access_points.push_back(added);
    emit access_point_added(added);
  });

  m_wifi_scanner->signal_removed.connect(
      [this](std::string removed) { handle_access_point_removed(removed); });

  m_wifi_state_watcher->signal_state_changed.connect(
      [this](const wifi::device_state state) {
        LOGGING_LOG(m_logger, logging::severity::normal)
            << "wifi device state changed:" << static_cast<int>(state);
        emit device_state_changed(state);
      });
}

const DataExchange::rooms &DataExchange::get_rooms() const { return m_rooms; }

const boost::optional<wolf::room> DataExchange::get_room(
    const wolf::types::uuid_array &id) const {
  auto found =
      std::find_if(m_rooms.begin(), m_rooms.end(),
                   [&id](const wolf::room &item) { return id == item.id; });
  if (found == m_rooms.cend()) {
    return {};
  }
  return *found;
}

// change method for using new sensor
bool DataExchange::moreThanOneConfiguration() const {
  if (m_rooms.size() > 1) return true;
  if (m_rooms.empty())
    throw std::runtime_error(
        "DataExchange::moreThanOneConfiguration: no rooms found!");

  // if there is co2, then there are humidity and/or temp too, because it is
  // scd30, ee895 or enocean
  bool voc = false;
  for (auto &sensor : m_rooms[0].sensors) {
    if (wolf::sensor_value_type_helper::is_voc(sensor)) voc = true;
    if (wolf::sensor_value_type_helper::is_co2(sensor)) return true;
  }

  // if there is voc but no co2, then there are only 2 configs if there is
  // humidity too (sht25 or shtc1)
  if (voc)
    for (auto &sensor : m_rooms[0].sensors) {
      if (wolf::sensor_value_type_helper::is_humidity(sensor)) return true;
    }

  // particles has more sensors und needs more views
  for (auto &sensor : m_rooms[0].sensors)
    if (wolf::sensor_value_type_helper::is_particle(sensor)) return true;

  // if it is an enocean sensor with light and no temperature it also has supply
  // voltage and needs 2 pages
  return contains_sensor(m_rooms[0].sensors, wolf::sensor_value_type::light) &&
         contains_sensor(m_rooms[0].sensors,
                         wolf::sensor_value_type::supply_voltage);
}

int DataExchange::get_room_count() const {
  return static_cast<int>(m_services.rooms->get_all().size());
}

DataExchange::configurations DataExchange::get_configurations() const {
  return m_configurations;
}

boost::optional<configuration> DataExchange::get_configuration(
    const wolf::types::uuid_array &id) const {
  auto found =
      std::find_if(m_configurations.begin(), m_configurations.end(),
                   [&id](const configuration &item) { return id == item.id; });
  if (found != m_configurations.cend()) {
    return *found;
  }
  return {};
}

boost::optional<configuration> DataExchange::get_configuration_by_room(
    const wolf::types::uuid_array &room_id) const {
  auto found = std::find_if(
      m_configurations.begin(), m_configurations.end(),
      [&room_id](const configuration &item) { return room_id == item.room; });
  if (found != m_configurations.cend()) {
    return *found;
  }
  return {};
}

const float &DataExchange::get_frsi() const { return m_frsi; }

const wolf::outdoor_sensor &DataExchange::get_outdoor_sensor_id() const {
  return m_outdoor_sensor_id;
}

const DataExchange::outdoor_values_type &DataExchange::get_outdoor_value()
    const {
  return m_outdoor_values;
}

const DataExchange::profiles &DataExchange::get_profiles() const {
  return m_profiles;
}

const DataExchange::values_type &DataExchange::get_values() const {
  return m_values;
}

void DataExchange::demand_foreign_sensors() {
  m_foreign_sensors.clear();
  m_services.service->post([this]() {
    if (!m_services.login_handler->is_logged_in()) return;
    m_services.foreign_sensor_requester->request(
        [this](const auto &foreign_sensors) {
          handle_foreign_sensors(foreign_sensors);
        });
  });
}

const DataExchange::foreign_sensors_type &DataExchange::get_foreign_sensors()
    const {
  return m_foreign_sensors;
}

boost::optional<configuration_values> DataExchange::get_configuration_values(
    const wolf::types::uuid_array &id) const {
  auto found = std::find_if(m_config_values.cbegin(), m_config_values.cend(),
                            [&id](const config_values::value_type &check) {
                              return check.configuration_id == id;
                            });
  if (found == m_config_values.cend())
    return boost::optional<configuration_values>();
  return *found;
}

boost::optional<configuration_state> DataExchange::get_configuration_state(
    const wolf::types::uuid_array &id) const {
  auto found = std::find_if(m_config_states.cbegin(), m_config_states.cend(),
                            [&id](const config_states::value_type &check) {
                              return check.first == id;
                            });
  if (found == m_config_states.cend()) return {};
  return found->second;
}

const DataExchange::config_states &DataExchange::get_all_configuration_states()
    const {
  return m_config_states;
}

float DataExchange::get_configuration_mold_value(
    const wolf::types::uuid_array &id) const {
  auto found = std::find_if(m_config_mold.cbegin(), m_config_mold.cend(),
                            [&id](const config_mold::value_type &check) {
                              return check.configuration == id;
                            });
  if (found == m_config_mold.cend()) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "should only happen with new rooms, id:" << id
        << ", m_config_mold.size():" << m_config_mold.size();
    return 0.f;
  }
  return found->percentage;
}

bool DataExchange::is_tone_enabled() const { return m_tone_enabled; }

bool DataExchange::is_nightmode_enabled() const { return m_nightmode_enabled; }

bool DataExchange::is_rotation_enabled() const { return m_rotation_enabled; }

const mold::WolfServices &DataExchange::getServices() const {
  return m_services;
}

DataExchange::limits DataExchange::get_limits(
    const wolf::types::uuid_array &room_id) const {
  auto found = find_controller_values(room_id);
  check_controller_values(found, "get_limits");
  return found->limits_;
}

DataExchange::actors DataExchange::get_actors(
    const wolf::types::uuid_array &room_id) const {
  auto found = find_controller_values(room_id);
  check_controller_values(found, "get_actors");
  return found->actors;
}

DataExchange::active_states DataExchange::get_active_states(
    const wolf::types::uuid_array &room_id) const {
  auto found = find_controller_values(room_id);
  check_controller_values(found, "get_active_states");
  return found->active_loops;
}

DataExchange::access_points DataExchange::get_access_points() const {
  return m_access_points;
}

boost::optional<std::string> DataExchange::get_active_access_point() {
  BOOST_ASSERT(m_active_getter);
  return m_active_getter->get();
}

void DataExchange::scan_wifi_networks() const {
  if (m_wifi_rescanner) m_wifi_rescanner->rescan();
}

bool DataExchange::is_wifi_hardware_available() const {
  return m_wifi_scanner != nullptr;
}

void DataExchange::handle_configuration_added(const configuration &config) {
  m_configurations.push_back(std::move(config));
}

void DataExchange::handle_configuration_updated(const configuration &config) {
  auto found = std::find_if(
      m_configurations.begin(), m_configurations.end(),
      [&config](const configuration &item) { return config.id == item.id; });
  if (found == m_configurations.cend())
    throw std::runtime_error(
        "DataExchange::handle_configuration_updated: ID not found!");
  (*found) = config;
}

void DataExchange::handle_configuration_removed(
    const wolf::types::uuid_array &id) {
  auto found =
      std::find_if(m_configurations.begin(), m_configurations.end(),
                   [&id](const configuration &item) { return id == item.id; });
  if (found == m_configurations.cend())
    throw std::runtime_error(
        "DataExchange::handle_configuration_removed: ID not found!");
  m_configurations.erase(found);
}

void DataExchange::handle_room_updated(const wolf::room &updated) {
  auto found = std::find_if(
      m_rooms.begin(), m_rooms.end(),
      [&updated](const wolf::room &check) { return check.id == updated.id; });
  if (found == m_rooms.cend())
    throw std::runtime_error("DataExchange::handle_room_update: ID not found!");
  *found = updated;
  emit room_updated(updated);
}

void DataExchange::handle_room_removed(const wolf::types::uuid_array &id) {
  auto found =
      std::find_if(m_rooms.begin(), m_rooms.end(),
                   [&id](const wolf::room &check) { return check.id == id; });
  if (found == m_rooms.cend())
    throw std::runtime_error(
        "DataExchange::handle_room_removed: ID not found!");
  m_rooms.erase(found);
  emit room_removed(id);
}

void DataExchange::handle_frsi_changed(const float &value) { m_frsi = value; }

void DataExchange::handle_foreign_sensors(
    const std::optional<DataExchange::foreign_sensors_type> &sensors) {
  if (!sensors) return;
  m_foreign_sensors = sensors.value();
  emit foreign_sensors();
}

DataExchange::values_list::const_iterator DataExchange::find_controller_values(
    const wolf::types::uuid_array &room_id) const {
  return std::find_if(
      m_controller_values.begin(), m_controller_values.end(),
      [&room_id](const wolf::controller::values_per_room &values) {
        return values.room_id == room_id;
      });
}

void DataExchange::check_controller_values(
    const DataExchange::values_list::const_iterator &found,
    const std::string &description) const {
  if (found == m_controller_values.end()) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not " << description
        << " because found == m_controller_values.end()";
    std::string error("DataExchange::");
    error += description;
    error += ": ID not found!";
    throw std::runtime_error(error);
  }
}

void DataExchange::handle_access_point_removed(const std::string &removed) {
  auto found = std::find_if(
      m_access_points.begin(), m_access_points.end(),
      [&removed](const access_point &check) { return check.path == removed; });
  if (found == m_access_points.cend())
    throw std::runtime_error(
        "DataExchange::handle_access_point_removed: path not found!");
  m_access_points.erase(found);
  emit access_point_removed(removed);
}

void DataExchange::handle_values(const wolf::sensor_values &values) {
  const auto all_values = m_services.value_handler->get_all();
  postToGUIThread([this, values, all_values] {
    m_values = all_values;
    // TODO this seems like a hack!
    emit generic_values();
  });
}

bool DataExchange::contains_sensor(const std::vector<wolf::sensor_id> &sensors,
                                   const wolf::sensor_value_type &type) const {
  return std::any_of(sensors.cbegin(), sensors.cend(),
                     [type](const wolf::sensor_id &sensor) {
                       return sensor.get_type() == type;
                     });
}
