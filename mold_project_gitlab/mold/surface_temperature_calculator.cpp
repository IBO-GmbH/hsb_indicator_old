#include "surface_temperature_calculator.hpp"

using namespace mold;

surface_temperature_calculator::surface_temperature_calculator(
    configuration_handler& configuration_handler_,
    options_handler& options_handler_)
    : m_configuration_handler_(configuration_handler_),
      m_options_handler(options_handler_) {}

float surface_temperature_calculator::calculate(
    const wolf::types::uuid_array& configuration_id,
    const temperatures& values) const {
  const float threshold_factor = get_threshold_factor(configuration_id);
  const float indoor_surface_temperature =
      values.outdoor + threshold_factor * (values.indoor - values.outdoor);
  return indoor_surface_temperature;
}

float surface_temperature_calculator::get_threshold_factor(
    const wolf::types::uuid_array& configuration_id) const {
  const auto configuration =
      m_configuration_handler_.get_or_throw(configuration_id);
  if (!configuration.mold_settings_ ||
      configuration.mold_settings_->building_type_ == building_type::none)
    return m_options_handler.get_frsi();
  return configuration.mold_settings_->frsi;
}
