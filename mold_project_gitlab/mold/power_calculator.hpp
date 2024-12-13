#ifndef MOLD_POWER_CALCULATOR_HPP
#define MOLD_POWER_CALCULATOR_HPP

#include <boost/asio/steady_timer.hpp>
#include "mold/room_saver.hpp"
#include "wolf/database.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace mold {
struct energy_value {
  wolf::sensor_value value;
  std::optional<wolf::sensor_value> last_total_power;
  using timestamp = std::chrono::system_clock::time_point;
  timestamp start_time;
};

class power_calculator {
 public:
  power_calculator(wolf::database& database,
                   boost::asio::io_service& io_service,
                   wolf::room_handler& room_handler,
                   wolf::sensor_value_handler& values_handler,
                   wolf::profile_handler& profile_handler,
                   const std::chrono::system_clock::duration& save_interval);
  void handle_room_added(const wolf::room& room);
  using sensors = std::vector<wolf::sensor_id>;
  void reset_energy(const sensors& sensor_ids);
  std::optional<energy_value::timestamp> get_start_time_energy(
      const wolf::sensor_id& sensor) const;
  void save_energy_to_database();
  void cancel_timer();

 private:
  using values = std::vector<wolf::sensor_value>;
  void handle_new_values(const values& sensor_values);
  void handle_current_sum(const values& current_values) const;
  static void remove_sum_values(values& values_for_sum);
  static std::vector<wolf::types::id_esp3> get_different_sender_ids(
      const values& values_for_sum);
  void calculate_sum_for_all_senders(const values& values_for_sum) const;
  static wolf::sensor_id change_sensor_id(const wolf::sensor_id& base_id,
                                          const wolf::sensor_value_type& type,
                                          const uint8_t subtype);

  void handle_room(const wolf::room& room);
  static bool check_if_room_has_sensor(const sensors& room_sensors,
                                       const wolf::sensor_value_type& to_check);
  static bool check_if_room_has_no_power(const sensors& room_sensors);
  static bool check_if_room_has_current_sum(const sensors& room_sensors);
  void add_power_sensors_to_room(const wolf::room& room_to_add_to);
  void add_current_sum_sensor_to_room(wolf::room& room_to_add_to) const;
  void add_energy_sensor_to_room(wolf::room& room_to_add_to) const;

  void calculate_power_values(const values& current_values) const;
  static wolf::sensor_id get_power_sensor_id(const wolf::sensor_id& current_id);

  static bool is_current_sum(const wolf::sensor_id& sensor);

  static std::optional<wolf::sensor_value> get_power_sum(
      const values& sensor_values);
  void calculate_energy(const wolf::sensor_value& power_sum);
  void add_new_energy_value(const wolf::sensor_value& power_sum,
                            const wolf::sensor_id& id);
  using energy_values = std::vector<energy_value>;
  void change_existing_energy_value(
      const wolf::sensor_value& power_sum,
      const energy_values::iterator& energy_value);
  void reset_energy(const wolf::sensor_id& sensor);

  void create_tables();
  void start_timer();
  void handle_wait(const boost::system::error_code& error);
  void remove_energy_from_database(
      const std::vector<wolf::types::id_esp3>& sensors_);
  void remove_from_database(const wolf::sensor_id& id);

  energy_values::const_iterator find_energy_value(
      const wolf::sensor_id& id) const;
  energy_values::iterator find_energy_value(const wolf::sensor_id& id);

  void load_from_database();

 private:
  mutable logging::logger m_logger;
  soci::session& m_session;
  wolf::room_handler& m_room_handler;
  wolf::sensor_value_handler& m_values_handler;
  wolf::profile_handler& m_profile_handler;

  room_saver m_room_saver;
  boost::asio::steady_timer m_save_timer;
  const std::chrono::system_clock::duration m_save_interval;
  energy_values m_energy_values;
};
}  // namespace mold

#endif  // MOLD_POWER_CALCULATOR_HPP
