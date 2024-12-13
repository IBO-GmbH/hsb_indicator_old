#include "outdoor_sensor.hpp"

std::ostream &wolf::operator<<(std::ostream &out,
                               const outdoor_sensor &to_print) {
  return out << "{temperature:" << to_print.temperature
             << ", humidity:" << to_print.humidity << "}";
}

using namespace wolf;

outdoor_sensor::outdoor_sensor(const sensor_id &temperature_,
                               const sensor_id &humidity_)
    : temperature(temperature_), humidity(humidity_) {}

bool outdoor_sensor::is_set() const {
  return !temperature.get_id().empty() || !humidity.get_id().empty();
}

bool wolf::operator==(const outdoor_sensor &first,
                      const outdoor_sensor &second) {
  return first.temperature == second.temperature &&
         first.humidity == second.humidity;
}

bool wolf::operator!=(const outdoor_sensor &first,
                      const outdoor_sensor &second) {
  return !(first == second);
}
