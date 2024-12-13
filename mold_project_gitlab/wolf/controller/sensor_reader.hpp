#ifndef WOLF_CONTROLLER_SENSOR_READER_HPP
#define WOLF_CONTROLLER_SENSOR_READER_HPP

#include "log/logger.hpp"
#include "sensor_value_getter.hpp"
#include "wolf/sensor_id.hpp"

namespace wolf {

namespace controller {

class sensor_reader {
 public:
  using sensors = std::vector<sensor_id>;
  using optional_float = boost::optional<float>;

  sensor_reader(sensor_value_getter& value_getter);
  optional_float read_humidity(const sensors& sensors);
  optional_float read_temperature(const sensors& sensors);
  void read_sensor_values_ventilation(const sensors& sensors,
                                      optional_float& relative_humidity,
                                      optional_float& temperature);
  optional_float get_absolute_humidity(const sensors& sensors,
                                       optional_float& relative_humidity,
                                       optional_float& temperature);
  optional_float get_absolute_humidity(const sensors& sensors,
                                       optional_float& relative_humidity);
  optional_float read_co2(const sensors& sensors);
  optional_float read_sprout_value(const sensors& sensors);
  optional_float read_voc(const sensors& sensors);
  bool get_sensor_values_svas4(const sensors& sensors, optional_float& co2,
                               optional_float& sprout_value,
                               optional_float& voc);

 private:
  logging::logger m_logger;
  sensor_value_getter& m_value_getter;
};
}  // namespace controller
}  // namespace wolf

#endif  // WOLF_CONTROLLER_SENSOR_READER_HPP
