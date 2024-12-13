#ifndef WOLF_SENSOR_METADATA_CREATOR_HPP
#define WOLF_SENSOR_METADATA_CREATOR_HPP

#include "sensor_metadata.hpp"
#include "sensor_type.hpp"
#include "types.hpp"

namespace wolf {
struct onboard_metadata {
  sensor_metadata temperature;
  sensor_metadata humidity_or_air_pressure;
  sensor_metadata co2;
};

class sensor_metadata_creator {
 public:
  static sensor_metadata enocean(const float min, const float max,
                                 const types::eep& eep,
                                 const types::id_esp3& id);
  static onboard_metadata sht25(const std::optional<sensor_type> second_sensor);
  static onboard_metadata shtc1(const std::optional<sensor_type> second_sensor);
  static onboard_metadata scd30(const std::optional<sensor_type> second_sensor);
  static sensor_metadata spgc3();
  static onboard_metadata ee895(
      const std::optional<sensor_type> temperature_sensor);
  static onboard_metadata sht85();
  static sensor_metadata tmp117();
  static void current_or_power(sensor_metadata& current_data,
                               const types::id& sensor_id);
  static sensor_metadata actor(const unsigned int actor_number,
                               const types::id_esp3& actor_id);
  using questionnaire_metadata = std::vector<sensor_metadata>;
  static questionnaire_metadata questionnaire();

 private:
  static sensor_metadata metadata_co2(
      const std::optional<sensor_type> co2_sensor);
  static sensor_metadata metadata_temperature(
      const std::optional<sensor_type> temperature_sensor);
};
}  // namespace wolf
#endif  // WOLF_SENSOR_METADATA_CREATOR_HPP
