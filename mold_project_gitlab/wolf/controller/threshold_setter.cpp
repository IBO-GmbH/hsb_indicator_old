#include "threshold_setter.hpp"
#include "calculator.hpp"

wolf::controller::threshold_setter::threshold_setter(
    const wolf::config &program_options)
    : m_program_options(program_options) {}

void wolf::controller::threshold_setter::set_absolute_humidity(
    const controller_per_room_list::iterator &found,
    const thresholds &absolute_humidity) {
  found->ventilation->set_absolute_humidity_thresholds(absolute_humidity);
}

void wolf::controller::threshold_setter::set_relative_humidity(
    const controller_per_room_list::iterator &found,
    const wolf::controller::limits::relative_humidity &limit_values) {
  found->ventilation->set_relative_humidity_thresholds(
      {m_program_options.controller_.relative_humidity_threshold,
       limit_values.max});
}

void wolf::controller::threshold_setter::set_thresholds_ventilation(
    const controller_per_room_list::iterator &found,
    const wolf::controller::limits::relative_humidity &relative_humidity,
    const thresholds &absolute_humidity) {
  found->ventilation->set_difference_humidity(relative_humidity.drH);
  set_absolute_humidity(found, absolute_humidity);
  set_relative_humidity(found, relative_humidity);
}

void wolf::controller::threshold_setter::set_humidity_border(
    const std::shared_ptr<svas2_dehumidifier> svas2,
    const float humidity_border, const float difference) {
  svas2->set_humidity_border(humidity_border);
  svas2->set_humidity_difference(difference);
}

void wolf::controller::threshold_setter::set_thresholds_temperature(
    const std::shared_ptr<svas3_heating> svas3,
    const wolf::controller::limits::temperature &limit_values) {
  svas3->set_limits(limit_values);
}

void wolf::controller::threshold_setter::set_thresholds_air_quality(
    const controller_per_room_list::iterator &found,
    const wolf::controller::limits::air_quality &limit_values) {
  set_co2(found, limit_values);
  set_sprout_value(found, limit_values);
  set_voc(found, limit_values);
}

void wolf::controller::threshold_setter::set_co2(
    const controller_per_room_list::iterator &found,
    const wolf::controller::limits::air_quality &limit_values) {
  found->svas4->set_co2_thresholds(
      {m_program_options.controller_.co2_threshold, limit_values.co2});
  found->svas4->set_co2_difference(limit_values.co2_difference);
}

void wolf::controller::threshold_setter::set_sprout_value(
    const controller_per_room_list::iterator &found,
    const wolf::controller::limits::air_quality &limit_values) {
  found->svas4->set_sprout_value_thresholds(
      {m_program_options.controller_.sprout_value_threshold,
       limit_values.sprout_value});
  found->svas4->set_sprout_value_difference(
      limit_values.sprout_value_difference);
}

void wolf::controller::threshold_setter::set_voc(
    const controller_per_room_list::iterator &found,
    const wolf::controller::limits::air_quality &limit_values) {
  found->svas4->set_voc_thresholds(
      {m_program_options.controller_.voc_threshold, limit_values.voc});
  found->svas4->set_voc_difference(limit_values.voc_difference);
}

void wolf::controller::threshold_setter::set_all(
    const controller_per_room_list::iterator &found,
    const wolf::controller::limits &limits,
    const thresholds &absolute_humidity) {
  set_thresholds_ventilation(found, limits.humidity, absolute_humidity);
  set_humidity_border(found->svas2, limits.humidity_border,
                      limits.humidity.drH);
  set_thresholds_temperature(found->svas3, limits.temperature_);
  set_thresholds_air_quality(found, limits.air_quality_);
}
