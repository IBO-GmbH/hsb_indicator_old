#ifndef WOLF_SENSOR_TYPE
#define WOLF_SENSOR_TYPE

#include <ostream>

namespace wolf {

enum struct sensor_type {
  // DON'T change order. affects sensor_id serialisation.
  none,
  enocean,
  sht25,
  shtc1,
  sprout_value,
  sgpc3,
  scd30,
  open_weather,
  evaluation_humidity,
  foreign,
  mold_state,
  frsi,
  substrate,
  sensor_rssi,
  actor_switch,
  ee895,
  comfort_questionnaire,
  mqtt,
  sht85,
  tmp117,
};
std::ostream& operator<<(std::ostream& os, const sensor_type out);
enum struct sensor_sht25_channel { temperature, humidity };
std::ostream& operator<<(std::ostream& os, const sensor_sht25_channel out);
enum struct sensor_shtc1_channel { temperature, humidity };
enum struct sensor_scd30_channel { temperature, humidity, co2 };
enum struct sensor_sgpc3_channel { tvoc };
enum struct sensor_ee895_channel { co2, temperature, air_pressure };
enum struct sensor_open_weather_channel {
  temperature = 0,
  humidity = 1,
  unknown
};
enum struct sensor_sht85_channel { temperature, humidity };
enum struct sensor_tmp117_channel { temperature };
}  // namespace wolf

#endif  // WOLF_SENSOR_TYPE
