#include "sensor_reader.hpp"
#include "calculator.hpp"
#include "wolf/sensor_value_type_helper.hpp"

using namespace logging;

wolf::controller::sensor_reader::sensor_reader(
    sensor_value_getter &value_getter)
    : m_logger("sensor_reader"), m_value_getter(value_getter) {}

wolf::controller::sensor_reader::optional_float
wolf::controller::sensor_reader::read_humidity(
    const wolf::controller::sensor_reader::sensors &sensors) {
  const auto found = std::find_if(sensors.cbegin(), sensors.cend(),
                                  &wolf::sensor_value_type_helper::is_humidity);
  if (found == sensors.cend()) return boost::none;
  return m_value_getter.get_humidity(*found);
}

wolf::controller::sensor_reader::optional_float
wolf::controller::sensor_reader::read_temperature(
    const wolf::controller::sensor_reader::sensors &sensors) {
  const auto found =
      std::find_if(sensors.cbegin(), sensors.cend(),
                   &wolf::sensor_value_type_helper::is_temperature);
  if (found == sensors.cend()) return boost::none;
  return m_value_getter.get_temperature(*found);
}

void wolf::controller::sensor_reader::read_sensor_values_ventilation(
    const wolf::controller::sensor_reader::sensors &sensors,
    optional_float &relative_humidity, optional_float &temperature) {
  relative_humidity = read_humidity(sensors);
  temperature = read_temperature(sensors);
}

wolf::controller::sensor_reader::optional_float
wolf::controller::sensor_reader::get_absolute_humidity(
    const wolf::controller::sensor_reader::sensors &sensors,
    optional_float &relative_humidity, optional_float &temperature) {
  read_sensor_values_ventilation(sensors, relative_humidity, temperature);
  if (relative_humidity && temperature) {
    auto absolute_humidity = calculator::calculate_absolute_humidity(
        temperature.get(), relative_humidity.get());
    return absolute_humidity;
  }
  return {};
}

wolf::controller::sensor_reader::optional_float
wolf::controller::sensor_reader::get_absolute_humidity(
    const wolf::controller::sensor_reader::sensors &sensors,
    optional_float &relative_humidity) {
  optional_float temperature;
  const auto absolute_humidity =
      get_absolute_humidity(sensors, relative_humidity, temperature);
  return absolute_humidity;
}

wolf::controller::sensor_reader::optional_float
wolf::controller::sensor_reader::read_co2(
    const wolf::controller::sensor_reader::sensors &sensors) {
  const auto found = std::find_if(sensors.cbegin(), sensors.cend(),
                                  &wolf::sensor_value_type_helper::is_co2);
  if (found == sensors.cend()) return boost::none;
  return m_value_getter.get_co2(*found);
}

wolf::controller::sensor_reader::optional_float
wolf::controller::sensor_reader::read_sprout_value(
    const wolf::controller::sensor_reader::sensors &sensors) {
  const auto found = std::find_if(sensors.cbegin(), sensors.cend(),
                                  &wolf::sensor_value_type_helper::is_sprout);
  if (found == sensors.cend()) return boost::none;
  return m_value_getter.get_sprout_value(*found);
}

wolf::controller::sensor_reader::optional_float
wolf::controller::sensor_reader::read_voc(
    const wolf::controller::sensor_reader::sensors &sensors) {
  const auto found = std::find_if(sensors.cbegin(), sensors.cend(),
                                  &wolf::sensor_value_type_helper::is_voc);
  if (found == sensors.cend()) return boost::none;
  return m_value_getter.get_voc(*found);
}

bool wolf::controller::sensor_reader::get_sensor_values_svas4(
    const wolf::controller::sensor_reader::sensors &sensors,
    optional_float &co2, optional_float &sprout_value, optional_float &voc) {
  co2 = read_co2(sensors);
  sprout_value = read_sprout_value(sensors);
  voc = read_voc(sensors);
  return (co2.has_value() || sprout_value.has_value() || voc.has_value());
}
