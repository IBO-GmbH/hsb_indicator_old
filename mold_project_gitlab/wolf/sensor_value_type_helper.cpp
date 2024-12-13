#include "sensor_value_type_helper.hpp"
#include <algorithm>
#include <boost/optional.hpp>

using namespace wolf;

bool sensor_value_type_helper::is_temperature(const sensor_id &id) {
  return id.get_type() == sensor_value_type::temperature;
}

bool sensor_value_type_helper::is_humidity(const sensor_id &id) {
  return id.get_type() == sensor_value_type::humidity;
}

bool sensor_value_type_helper::is_co2(const sensor_id &id) {
  return id.get_type() == sensor_value_type::co2;
}

bool sensor_value_type_helper::is_voc(const sensor_id &id) {
  return id.get_type() == sensor_value_type::voc;
}

bool sensor_value_type_helper::is_sprout(const sensor_id &id) {
  return id.get_type() == sensor_value_type::sprout;
}

bool sensor_value_type_helper::is_particle(const sensor_id &id) {
  return id.get_type() == sensor_value_type::particles_pm10 ||
         id.get_type() == sensor_value_type::particles_pm2p5 ||
         id.get_type() == sensor_value_type::particles_pm1;
}

bool sensor_value_type_helper::is_evaluation_humidity(const sensor_id &id) {
  return id.get_type() == sensor_value_type::evaluation_humidity;
}

bool sensor_value_type_helper::is_electric_current(const sensor_id &id) {
  return id.get_type() == sensor_value_type::electric_current;
}

bool sensor_value_type_helper::is_electric_power(const sensor_id &id) {
  return id.get_type() == sensor_value_type::electric_power;
}

bool sensor_value_type_helper::is_electric_energy(const sensor_id &id) {
  return id.get_type() == sensor_value_type::electric_energy;
}

bool sensor_value_type_helper::is_air_pressure(const sensor_id &id) {
  return id.get_type() == sensor_value_type::air_pressure;
}

bool sensor_value_type_helper::is_light(const sensor_id &id) {
  return id.get_type() == sensor_value_type::light;
}

bool sensor_value_type_helper::is_magnet_switch(const sensor_id &id) {
  return id.get_type() == sensor_value_type::magnet_switch;
}

bool sensor_value_type_helper::is_motion(const sensor_id &id) {
  return id.get_type() == sensor_value_type::motion;
}

bool sensor_value_type_helper::is_acceleration(const sensor_id &id) {
  return id.get_type() == sensor_value_type::acceleration;
}

bool sensor_value_type_helper::is_supply_voltage(const sensor_id &id) {
  return id.get_type() == sensor_value_type::supply_voltage;
}

bool sensor_value_type_helper::is_presence(const sensor_id &id) {
  return id.get_type() == sensor_value_type::presence;
}

boost::optional<sensor_id> sensor_value_type_helper::find_temperature_in_list(
    const find_in_listcontainer &container) {
  const auto found = std::find_if(
      container.cbegin(), container.cend(),
      [](const wolf::sensor_id &id) { return is_temperature(id); });
  if (found == container.cend()) return {};
  return *found;
}

boost::optional<sensor_id> sensor_value_type_helper::find_humidity_in_list(
    const find_in_listcontainer &container) {
  const auto found =
      std::find_if(container.cbegin(), container.cend(),
                   [](const wolf::sensor_id &id) { return is_humidity(id); });
  if (found == container.cend()) return {};
  return *found;
}
