#ifndef WOLF_SENSOR_VALUE_TYPE_HPP
#define WOLF_SENSOR_VALUE_TYPE_HPP

#include <ostream>

namespace wolf {

enum class sensor_value_type {
  // DON'T change order. affects sensor_id serialisation.
  unknown,
  temperature,
  humidity,
  co2,
  voc,
  sprout,
  evaluation_humidity,
  mold_state,
  frsi,
  particles_pm10,
  particles_pm2p5,
  particles_pm1,
  substrate,
  sensor_rssi,
  electric_current,
  electric_power,
  electric_energy,
  electric_energy_difference,
  air_pressure,
  actor_switch,
  light,
  magnet_switch,
  motion,
  acceleration,
  supply_voltage,
  presence,
  questionnaire_gender,
  questionnaire_age,
  questionnaire_weight,
  questionnaire_height,
  questionnaire_clothing,
  questionnaire_temperature,
  questionnaire_air_quality,
  questionnaire_humidity,
  questionnaire_activity,
  questionnaire_thermic_comfort,
  questionnaire_heating,
  valve_temperature,
  valve_position,
};

std::ostream& operator<<(std::ostream& out, const sensor_value_type to_print);

}  // namespace wolf

#endif
