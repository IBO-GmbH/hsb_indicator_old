#include "onboard_sensor_checker.hpp"

#include <algorithm>

#include "sensor_id_sgpc3.hpp"
#include "wolf/sensor_id_ee895.hpp"
#include "wolf/sensor_id_scd30.hpp"
#include "wolf/sensor_id_sht25.hpp"
#include "wolf/sensor_id_sht85.hpp"
#include "wolf/sensor_id_shtc1.hpp"
#include "wolf/sensor_id_tmp117.hpp"

bool wolf::onboard_sensor_checker::has_onboard_sensor(const wolf::room &room_) {
  return has_sht25(room_) || has_shtc1(room_) || has_scd30(room_) ||
         has_sgpc3(room_) || has_ee895(room_);
}

bool wolf::onboard_sensor_checker::has_sht25(const wolf::room &room_) {
  auto check_temperature =
      wolf::sensor_id_sht25::create(wolf::sensor_sht25_channel::temperature);
  auto check_humidity =
      wolf::sensor_id_sht25::create(wolf::sensor_sht25_channel::humidity);
  return check_sensor(room_, {check_temperature, check_humidity});
}

bool wolf::onboard_sensor_checker::has_shtc1(const wolf::room &room_) {
  auto check_temperature =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::temperature);
  auto check_humidity =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::humidity);
  return check_sensor(room_, {check_temperature, check_humidity});
}

bool wolf::onboard_sensor_checker::has_scd30(const wolf::room &room_) {
  auto check_temperature =
      wolf::sensor_id_scd30::create(wolf::sensor_scd30_channel::temperature);
  auto check_humidity =
      wolf::sensor_id_scd30::create(wolf::sensor_scd30_channel::humidity);
  auto check_co2 =
      wolf::sensor_id_scd30::create(wolf::sensor_scd30_channel::co2);
  return check_sensor(room_, {check_temperature, check_humidity, check_co2});
}

bool wolf::onboard_sensor_checker::has_sgpc3(const room &room_) {
  auto check_voc =
      wolf::sensor_id_sgpc3::create(wolf::sensor_sgpc3_channel::tvoc);
  return check_sensor(room_, {check_voc});
}

bool wolf::onboard_sensor_checker::has_ee895(const wolf::room &room_) {
  auto check_co2 =
      wolf::sensor_id_ee895::create(wolf::sensor_ee895_channel::co2);
  auto check_temperature =
      wolf::sensor_id_ee895::create(wolf::sensor_ee895_channel::temperature);
  auto check_air_pressure =
      wolf::sensor_id_ee895::create(wolf::sensor_ee895_channel::air_pressure);
  return check_sensor(room_,
                      {check_co2, check_temperature, check_air_pressure});
}

bool wolf::onboard_sensor_checker::has_sht85(const wolf::room &room_) {
  auto check_temperature =
      wolf::sensor_id_sht85::create(wolf::sensor_sht85_channel::temperature);
  auto check_humidity =
      wolf::sensor_id_sht85::create(wolf::sensor_sht85_channel::humidity);
  return check_sensor(room_, {check_temperature, check_humidity});
}

bool wolf::onboard_sensor_checker::has_tmp117(const wolf::room &room_) {
  auto check_temperature =
      wolf::sensor_id_tmp117::create(wolf::sensor_tmp117_channel::temperature);
  return check_sensor(room_, {check_temperature});
}

bool wolf::onboard_sensor_checker::check_sensor(
    const wolf::room &room_,
    const wolf::onboard_sensor_checker::sensors &to_check) {
  auto found = std::find_if(room_.sensors.begin(), room_.sensors.end(),
                            [&to_check](const wolf::sensor_id &sensor) {
                              for (const auto &check_sensor : to_check)
                                if (sensor == check_sensor) return true;
                              return false;
                            });
  return !(found == room_.sensors.end());
}
