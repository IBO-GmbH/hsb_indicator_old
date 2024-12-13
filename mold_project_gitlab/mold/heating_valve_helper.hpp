#ifndef MOLD_HEATING_VALVE_HELPER_HPP
#define MOLD_HEATING_VALVE_HELPER_HPP

#include "mold/heating_valve_settings.hpp"
#include "wolf/sensor_value.hpp"
#include "wolf/sensor_value_handler.hpp"

#include <optional>

namespace mold {

class heating_valve_helper {
 public:
  static bool is_heating_valve(const wolf::sensor_id &id);
  using enocean_ids = std::vector<wolf::types::id_esp3>;
  using sensor_ids = std::vector<wolf::sensor_id>;
  static enocean_ids get_all_heating_valve_enocean_ids(
      const sensor_ids &sensors);
  static bool is_enocean_id_in_list(const wolf::types::id_esp3 &id,
                                    const enocean_ids &list);
  static sensor_ids get_all_heating_valve_sensor_ids(const sensor_ids &sensors);
  static wolf::sensor_values get_heating_valve_values(
      const wolf::sensor_values &all_values, const wolf::types::id_esp3 &id);
  static wolf::types::data get_data_bytes(
      const bool init, const heating_valve_settings &settings,
      const std::optional<float> &current_temperature);
  static wolf::types::data get_learn_response_bytes();
  static std::optional<float> get_current_room_temperature_value(
      const sensor_ids &sensors, wolf::sensor_value_handler &value_handler);

 private:
  static std::uint8_t calculate_temperature_byte(const float temperature);
  static std::uint8_t get_byte_zero();
  static std::uint8_t get_byte_one(const bool init, const bool summer_mode,
                                   const bool manual);
  static std::uint8_t get_byte_two(
      const std::optional<float> &current_temperature);
  static std::uint8_t get_byte_three(const float temperature,
                                     const std::uint8_t &valve_position,
                                     const bool manual);
  static std::optional<wolf::sensor_id> get_current_room_temperature_sensor(
      const sensor_ids &sensors);
};
}  // namespace mold

#endif  // MOLD_HEATING_VALVE_HELPER_HPP
