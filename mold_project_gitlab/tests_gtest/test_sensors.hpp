#ifndef TEST_SENSORS_HPP
#define TEST_SENSORS_HPP

#include "wolf/outdoor_sensor.hpp"
#include "wolf/sensor_id.hpp"

namespace test_sensors {

struct indoor {
  wolf::sensor_id temperature;
  wolf::sensor_id humidity;
};
indoor create_indoor_sensors();
wolf::outdoor_sensor create_outdoor_sensor();

}  // namespace test_sensors

#endif
