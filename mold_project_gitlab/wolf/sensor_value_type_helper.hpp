#ifndef WOLF_SENSOR_VALUE_TYPE_HELPER_HPP
#define WOLF_SENSOR_VALUE_TYPE_HELPER_HPP

#include "wolf/sensor_id.hpp"

#include <boost/config.hpp>
#include <boost/optional/optional_fwd.hpp>

namespace wolf {

class sensor_value_type_helper {
 public:
  static bool is_temperature(const sensor_id& id);
  static bool is_humidity(const sensor_id& id);
  static bool is_co2(const sensor_id& id);
  static bool is_voc(const sensor_id& id);
  static bool is_sprout(const sensor_id& id);
  static bool is_particle(const sensor_id& id);
  static bool is_evaluation_humidity(const sensor_id& id);
  static bool is_electric_current(const sensor_id& id);
  static bool is_electric_power(const sensor_id& id);
  static bool is_electric_energy(const sensor_id& id);
  static bool is_air_pressure(const sensor_id& id);
  static bool is_light(const sensor_id& id);
  static bool is_magnet_switch(const sensor_id& id);
  static bool is_motion(const sensor_id& id);
  static bool is_acceleration(const sensor_id& id);
  static bool is_supply_voltage(const sensor_id& id);
  static bool is_presence(const sensor_id& id);

  using find_in_listcontainer = std::vector<sensor_id>;
  static boost::optional<sensor_id> find_temperature_in_list(
      const find_in_listcontainer& container);
  static boost::optional<sensor_id> find_humidity_in_list(
      const find_in_listcontainer& container);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_CHANNEL_HELPER_HPP
