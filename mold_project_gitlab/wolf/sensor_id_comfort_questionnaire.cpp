#include "sensor_id_comfort_questionnaire.hpp"

using namespace wolf;

wolf::sensor_id wolf::sensor_id_comfort_questionnaire::create(
    const wolf::sensor_value_type &question) {
  types::id id(2);
  id.front() = static_cast<std::uint8_t>(sensor_type::comfort_questionnaire);
  id[1] = static_cast<std::uint8_t>(question);
  return sensor_id{id, question};
}
