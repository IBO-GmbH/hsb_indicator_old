#include "sensor_value_type.hpp"
#include <boost/assert.hpp>

std::ostream& wolf::operator<<(std::ostream& out,
                               const sensor_value_type to_print) {
  switch (to_print) {
    case sensor_value_type::unknown:
      return out << "unknown";
    case sensor_value_type::temperature:
      return out << "temperature";
    case sensor_value_type::humidity:
      return out << "humidity";
    case sensor_value_type::co2:
      return out << "co2";
    case sensor_value_type::voc:
      return out << "voc";
    case sensor_value_type::sprout:
      return out << "sprout";
    case sensor_value_type::particles_pm10:
      return out << "particles_pm10";
    case sensor_value_type::particles_pm2p5:
      return out << "particles_pm2p5";
    case sensor_value_type::particles_pm1:
      return out << "particles_pm1";
    case sensor_value_type::evaluation_humidity:
      return out << "evaluation_humidity";
    case sensor_value_type::mold_state:
      return out << "mold_state";
    case sensor_value_type::frsi:
      return out << "frsi";
    case wolf::sensor_value_type::substrate:
      return out << "substrate";
    case wolf::sensor_value_type::sensor_rssi:
      return out << "sensor_rssi";
    case wolf::sensor_value_type::electric_current:
      return out << "electric_current";
    case wolf::sensor_value_type::electric_power:
      return out << "electric_power";
    case wolf::sensor_value_type::electric_energy:
      return out << "electric_energy";
    case wolf::sensor_value_type::electric_energy_difference:
      return out << "electric_energy_difference";
    case wolf::sensor_value_type::air_pressure:
      return out << "air_pressure";
    case wolf::sensor_value_type::actor_switch:
      return out << "actor_switch";
    case wolf::sensor_value_type::light:
      return out << "light";
    case wolf::sensor_value_type::magnet_switch:
      return out << "magnet_switch";
    case wolf::sensor_value_type::motion:
      return out << "motion";
    case wolf::sensor_value_type::acceleration:
      return out << "acceleration";
    case wolf::sensor_value_type::supply_voltage:
      return out << "supply_voltage";
    case wolf::sensor_value_type::presence:
      return out << "presence";
    case wolf::sensor_value_type::questionnaire_gender:
      return out << "questionnaire_gender";
    case wolf::sensor_value_type::questionnaire_age:
      return out << "questionnaire_age";
    case wolf::sensor_value_type::questionnaire_weight:
      return out << "questionnaire_weight";
    case wolf::sensor_value_type::questionnaire_height:
      return out << "questionnaire_height";
    case wolf::sensor_value_type::questionnaire_clothing:
      return out << "questionnaire_clothing";
    case wolf::sensor_value_type::questionnaire_temperature:
      return out << "questionnaire_temperature";
    case wolf::sensor_value_type::questionnaire_air_quality:
      return out << "questionnaire_air_quality";
    case wolf::sensor_value_type::questionnaire_humidity:
      return out << "questionnaire_humidity";
    case wolf::sensor_value_type::questionnaire_activity:
      return out << "questionnaire_activity";
    case wolf::sensor_value_type::questionnaire_thermic_comfort:
      return out << "questionnaire_thermic_comfort";
    case wolf::sensor_value_type::questionnaire_heating:
      return out << "questionnaire_heating";
    case wolf::sensor_value_type::valve_temperature:
      return out << "valve_temperature";
    case wolf::sensor_value_type::valve_position:
      return out << "valve_position";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}
