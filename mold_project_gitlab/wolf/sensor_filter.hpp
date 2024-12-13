#ifndef WOLF_SENSOR_FILTER_HPP
#define WOLF_SENSOR_FILTER_HPP

#include "low_pass_filter.hpp"
#include "sensor_type.hpp"

namespace wolf {

class sensor_filter {
 public:
  sensor_filter();
  bool filter_value(const wolf::sensor_type &type, low_pass_filter &filter,
                    float &value);

 private:
  logging::logger m_logger;
};
}  // namespace wolf

#endif  // WOLF_SENSOR_FILTER_HPP
