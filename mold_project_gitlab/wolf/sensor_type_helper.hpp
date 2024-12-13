#ifndef WOLF_SENSOR_TYPE_HELPER_HPP
#define WOLF_SENSOR_TYPE_HELPER_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_type_helper {
 public:
  static sensor_type get_type(const sensor_id& id);
  static bool is_none(const sensor_id& id);
  static bool is_enocean(const sensor_id& id);
  static bool is_sht25(const sensor_id& id);
  static bool is_shtc1(const sensor_id& id);
  static bool is_sprout_value(const sensor_id& id);
  static bool is_sgpc3(const sensor_id& id);
  static bool is_scd30(const sensor_id& id);
  static bool is_ee895(const sensor_id& id);
  static bool is_open_weather(const sensor_id& id);
  static bool is_evaluation_humidity(const sensor_id& id);
  static bool is_foreign(const sensor_id& id);
  static bool is_mold_state(const sensor_id& id);
  static bool is_frsi(const sensor_id& id);
  static bool is_substrate(const sensor_id& id);
  static bool is_sensor_rssi(const sensor_id& id);
  static bool is_comfort_questionnaire(const sensor_id& id);
  static bool is_sht85(const sensor_id& id);
  static bool is_tmp117(const sensor_id& id);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_TYPE_HELPER_HPP
