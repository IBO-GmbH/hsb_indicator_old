#ifndef WOLF_SENSOR_ID_COMFORT_QUESTIONNAIRE_HPP
#define WOLF_SENSOR_ID_COMFORT_QUESTIONNAIRE_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_id_comfort_questionnaire {
 public:
  static sensor_id create(const sensor_value_type& question);
};
}  // namespace wolf
#endif  // WOLF_SENSOR_ID_COMFORT_QUESTIONNAIRE_HPP
