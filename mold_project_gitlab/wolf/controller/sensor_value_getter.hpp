#ifndef WOLF_CONTROLLER_SENSOR_VALUE_GETTER_HPP
#define WOLF_CONTROLLER_SENSOR_VALUE_GETTER_HPP

#include "log/logger.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace wolf {

namespace controller {

class sensor_value_getter {
 public:
  sensor_value_getter(sensor_value_handler &values_handler_);
  boost::optional<float> get_humidity(const wolf::sensor_id &sensor);
  boost::optional<float> get_temperature(const wolf::sensor_id &sensor);
  boost::optional<float> get_co2(const wolf::sensor_id &sensor);
  boost::optional<float> get_sprout_value(const wolf::sensor_id &sensor);
  boost::optional<float> get_voc(const wolf::sensor_id &sensor);

 private:
  boost::optional<float> get_value(const wolf::sensor_id &sensor);

 private:
  logging::logger m_logger;
  sensor_value_handler &m_values_handler;
};
}  // namespace controller
}  // namespace wolf

#endif  // WOLF_CONTROLLER_SENSOR_VALUE_GETTER_HPP
