#include "limits.hpp"

#include "wolf/math_compare_decimals.hpp"

std::ostream &wolf::controller::operator<<(
    std::ostream &out, const wolf::controller::limits &to_print) {
  out << "{relative humidity: {max: " << to_print.humidity.max
      << ", drH: " << to_print.humidity.drH << "}, humidity_border: {"
      << to_print.humidity_border
      << "}, temperature: {min: " << to_print.temperature_.min
      << ", reference: " << to_print.temperature_.reference
      << "}, air quality: {co2: " << to_print.air_quality_.co2
      << ", co2_difference: " << to_print.air_quality_.co2_difference
      << ", sprout_value: " << to_print.air_quality_.sprout_value
      << ", sprout_value_difference: "
      << to_print.air_quality_.sprout_value_difference
      << ", voc: " << to_print.air_quality_.voc
      << ", voc_difference: " << to_print.air_quality_.voc_difference << "}"
      << ", outdoor_temperature: {" << to_print.outdoor_temperature;
  return out << "}";
}

bool wolf::controller::operator==(
    const wolf::controller::limits::relative_humidity &first,
    const wolf::controller::limits::relative_humidity &second) {
  return wolf::math::compare_floats(first.max, second.max) &&
         wolf::math::compare_floats(first.drH, second.drH);
}

bool wolf::controller::operator!=(
    const wolf::controller::limits::relative_humidity &first,
    const wolf::controller::limits::relative_humidity &second) {
  return !(first == second);
}

bool wolf::controller::operator==(
    const wolf::controller::limits::temperature &first,
    const wolf::controller::limits::temperature &second) {
  return wolf::math::compare_floats(first.min, second.min) &&
         wolf::math::compare_floats(first.reference, second.reference);
}

bool wolf::controller::operator!=(
    const wolf::controller::limits::temperature &first,
    const wolf::controller::limits::temperature &second) {
  return !(first == second);
}

bool wolf::controller::operator==(
    const wolf::controller::limits::air_quality &first,
    const wolf::controller::limits::air_quality &second) {
  return wolf::math::compare_floats(first.co2, second.co2) &&
         wolf::math::compare_floats(first.co2_difference,
                                    second.co2_difference) &&
         wolf::math::compare_floats(first.sprout_value, second.sprout_value) &&
         wolf::math::compare_floats(first.sprout_value_difference,
                                    second.sprout_value_difference) &&
         wolf::math::compare_floats(first.voc, second.voc) &&
         wolf::math::compare_floats(first.voc_difference,
                                    second.voc_difference);
}

bool wolf::controller::operator!=(
    const wolf::controller::limits::air_quality &first,
    const wolf::controller::limits::air_quality &second) {
  return !(first == second);
}

bool wolf::controller::operator==(const wolf::controller::limits &first,
                                  const wolf::controller::limits &second) {
  return first.humidity == second.humidity &&
         wolf::math::compare_floats(first.humidity_border,
                                    second.humidity_border) &&
         first.temperature_ == second.temperature_ &&
         first.air_quality_ == second.air_quality_ &&
         wolf::math::compare_floats(first.outdoor_temperature,
                                    second.outdoor_temperature);
}

bool wolf::controller::operator!=(const wolf::controller::limits &first,
                                  const wolf::controller::limits &second) {
  return !(first == second);
}
