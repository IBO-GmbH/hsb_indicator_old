#include "test_sensors.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_id_shtc1.hpp"

test_sensors::indoor test_sensors::create_indoor_sensors() {
  indoor result;
  result.temperature =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::temperature);
  result.humidity =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::humidity);
  return result;
}

wolf::outdoor_sensor test_sensors::create_outdoor_sensor() {
  wolf::outdoor_sensor result;
  result.temperature = wolf::sensor_id_enocean::create(
      1, S_TEMP, std::optional<wolf::enocean_subchannel_type>(), {});
  result.humidity = wolf::sensor_id_enocean::create(
      1, S_RELHUM, std::optional<wolf::enocean_subchannel_type>(), {});
  return result;
}
