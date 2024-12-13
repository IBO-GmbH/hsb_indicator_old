#include "sensor_value_getter.hpp"

#include "wolf/sensor_value_type_helper.hpp"

using namespace wolf;
using namespace controller;
using namespace logging;

sensor_value_getter::sensor_value_getter(sensor_value_handler &values_handler_)
    : m_logger("sensor_value_getter"), m_values_handler(values_handler_) {}

boost::optional<float> sensor_value_getter::get_humidity(
    const sensor_id &sensor) {
  if (!sensor_value_type_helper::is_humidity(sensor)) return boost::none;
  return get_value(sensor);
}

boost::optional<float> sensor_value_getter::get_temperature(
    const sensor_id &sensor) {
  if (!sensor_value_type_helper::is_temperature(sensor)) return boost::none;
  return get_value(sensor);
}

boost::optional<float> sensor_value_getter::get_co2(const sensor_id &sensor) {
  if (!sensor_value_type_helper::is_co2(sensor)) return boost::none;
  return get_value(sensor);
}

boost::optional<float> sensor_value_getter::get_sprout_value(
    const sensor_id &sensor) {
  if (!sensor_value_type_helper::is_sprout(sensor)) return boost::none;
  return get_value(sensor);
}

boost::optional<float> sensor_value_getter::get_voc(const sensor_id &sensor) {
  if (!sensor_value_type_helper::is_voc(sensor)) return boost::none;
  return get_value(sensor);
}

boost::optional<float> sensor_value_getter::get_value(const sensor_id &sensor) {
  auto value = m_values_handler.get_value_float(sensor);
  return value;
}
