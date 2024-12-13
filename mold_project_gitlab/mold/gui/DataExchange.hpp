#ifndef MOLD_GUI_DATAEXCHANGE_HPP
#define MOLD_GUI_DATAEXCHANGE_HPP

#include <QObject>
#include "mold/configuration_values_handler.hpp"
#include "mold/gui/WolfServices.hpp"
#include "wifi/active_access_point_getter.hpp"
#include "wifi/device_state_watcher.hpp"
#include "wifi/rescanner.hpp"
#include "wifi/scanner.hpp"
#include "wolf/foreign_sensor.hpp"

namespace mold {

class DataExchange : public QObject {
  Q_OBJECT
 public:
  DataExchange(const mold::WolfServices& services);
  DataExchange(DataExchange&) = delete;

  void initialise();

  using configurations = std::vector<mold::configuration>;
  using rooms = std::vector<wolf::room>;
  using profiles = std::vector<wolf::sensor_profile>;

  virtual const rooms& get_rooms() const;
  const boost::optional<wolf::room> get_room(
      const wolf::types::uuid_array& id) const;
  bool moreThanOneConfiguration() const;
  int get_room_count() const;

  configurations get_configurations() const;
  boost::optional<configuration> get_configuration(
      const wolf::types::uuid_array& id) const;
  boost::optional<configuration> get_configuration_by_room(
      const wolf::types::uuid_array& room_id) const;

  const float& get_frsi() const;
  const wolf::outdoor_sensor& get_outdoor_sensor_id() const;
  using outdoor_values_type = configuration_values_outdoor;
  const outdoor_values_type& get_outdoor_value() const;

  const profiles& get_profiles() const;

  using values_type = std::vector<wolf::sensor_value>;
  const values_type& get_values() const;

  void demand_foreign_sensors();
  using foreign_sensors_type = std::vector<wolf::foreign_sensor>;
  const foreign_sensors_type& get_foreign_sensors() const;

  boost::optional<configuration_values> get_configuration_values(
      const wolf::types::uuid_array& id) const;
  boost::optional<configuration_state> get_configuration_state(
      const wolf::types::uuid_array& id) const;
  using config_states =
      std::vector<std::pair<wolf::types::uuid_array, configuration_state>>;
  const config_states& get_all_configuration_states() const;
  float get_configuration_mold_value(const wolf::types::uuid_array& id) const;

  bool is_tone_enabled() const;
  bool is_nightmode_enabled() const;
  bool is_rotation_enabled() const;

  const mold::WolfServices& getServices() const;

  using values_list = std::vector<wolf::controller::values_per_room>;
  using limits = wolf::controller::limits;
  using actors = wolf::controller::controller::actor_array;
  using active_states = wolf::controller::values_per_room::active_states;

  limits get_limits(const wolf::types::uuid_array& room_id) const;
  actors get_actors(const wolf::types::uuid_array& room_id) const;
  active_states get_active_states(const wolf::types::uuid_array& room_id) const;

  using access_point = wifi::access_point;
  using access_points = std::vector<access_point>;
  access_points get_access_points() const;
  boost::optional<std::string> get_active_access_point();

  void scan_wifi_networks() const;
  bool is_wifi_hardware_available() const;

 signals:
  void configuration_outdoor_sensor_changed(
      const wolf::outdoor_sensor& sensor_id);
  void configuration_values_changed();
  void configuration_molds_changed();
  void configuration_states_changed();

  void profiles_updated();

  void room_added(const wolf::room& room_);
  void room_updated(const wolf::room& room_);
  void room_removed(const wolf::types::uuid_array& room_id);

  void generic_values();

  void learned(const wolf::types::id_esp3 id, const std::uint8_t& org,
               const std::uint8_t& func, const std::uint8_t& type);
  void data_4bs_or_vld(const wolf::types::id_esp3& id,
                       const wolf::types::data& data, const int rssi);
  void foreign_sensors();

  void display_on_changed(const bool on);

  void access_point_added(const access_point& added);
  void access_point_removed(const std::string& removed);
  void device_state_changed(const wifi::device_state state);

  void controller_updated();
  void enocean_values_read(const wolf::sensor_id& id, const int rssi,
                           const rooms& rooms_);

 private:
  void initialise_wifi();

  void handle_configuration_added(const mold::configuration& config);
  void handle_configuration_updated(const configuration& config);
  void handle_configuration_removed(const wolf::types::uuid_array& id);

  void handle_room_updated(const wolf::room& updated);
  void handle_room_removed(const wolf::types::uuid_array& id);

  void handle_frsi_changed(const float& value);

  void handle_foreign_sensors(
      const std::optional<foreign_sensors_type>& sensors);

  values_list::const_iterator find_controller_values(
      const wolf::types::uuid_array& room_id) const;
  void check_controller_values(const values_list::const_iterator& found,
                               const std::string& description) const;

  void handle_access_point_removed(const std::string& removed);

  void handle_values(const wolf::sensor_values& values);

  bool contains_sensor(const std::vector<wolf::sensor_id>& sensors,
                       const wolf::sensor_value_type& type) const;

 private:
  mutable logging::logger m_logger;

  const mold::WolfServices& m_services;

  rooms m_rooms;
  configurations m_configurations;
  wolf::outdoor_sensor m_outdoor_sensor_id;
  outdoor_values_type m_outdoor_values;
  float m_frsi;

  using config_values = std::vector<configuration_values>;
  config_values m_config_values;
  config_states m_config_states;
  using config_mold = std::vector<mold_value>;
  config_mold m_config_mold;

  values_type m_values;

  foreign_sensors_type m_foreign_sensors;
  profiles m_profiles;

  bool m_tone_enabled;
  bool m_nightmode_enabled;
  bool m_rotation_enabled;

  values_list m_controller_values;

  std::unique_ptr<wifi::scanner> m_wifi_scanner;
  std::unique_ptr<wifi::rescanner> m_wifi_rescanner;
  std::unique_ptr<wifi::device_state_watcher> m_wifi_state_watcher;
  std::unique_ptr<wifi::active_access_point_getter> m_active_getter;
  access_points m_access_points;
};
}  // namespace mold

#endif  // MOLD_GUI_DATAEXCHANGE_HPP
