#ifndef MOLD_SPROUT_HANDLER_HPP
#define MOLD_SPROUT_HANDLER_HPP

#include "mold/configuration_values_handler.hpp"
#include "mold/sprout/cache.hpp"
#include "mold/sprout/data_types.hpp"
#include "mold/sprout_value.hpp"
#include "mold/surface_temperature_calculator.hpp"

namespace mold::sprout {

class handler {
 public:
  using time_point = std::chrono::system_clock::time_point;

  handler(cache& cache_, configuration_values_handler& values,
          surface_temperature_calculator& surface_temperature_calculator_);

  void add_configuration(const wolf::types::uuid_array& id);
  void remove_configuration(const wolf::types::uuid_array& id);
  void handle_evaluation_humidity(const wolf::types::uuid_array& id,
                                  const wolf::sensor_value& value,
                                  const substrate_type& type);
  data_types::value& get(const wolf::types::uuid_array& id);
  const data_types::values& get_all() const;
  boost::signals2::signal<void(const data_types::value&)>
      signal_sprout_type_changed;

 private:
  using border_points_type = std::array<float, 6>;
  void calculate(const wolf::types::uuid_array& id,
                 const wolf::sensor_value& evaluation_humidity,
                 const configuration_values& values,
                 const configuration_values_handler::outdoor_values& outdoor,
                 const substrate_type& type);
  void calculate(const wolf::types::uuid_array& id,
                 const border_points_type& border_points_,
                 const wolf::sensor_value& evaluation_humidity);
  sprout_type type_from_border_points(const border_points_type& border_points_,
                                      const float evaluation_humidity) const;
  border_points_type border_points(
      const wolf::types::uuid_array& id, const configuration_values& values,
      const configuration_values_handler::outdoor_values& outdoor,
      const substrate_type& type);
  data_types::values::iterator find_by_configuration(
      const wolf::types::uuid_array& configuration);
  void load_all();
  void save_all();
  bool handle_initial_change(data_types::value& value, const time_point& now);
  void handle_curve_change_timestamps(data_types::value& value,
                                      const sprout_type& type,
                                      const time_point& now);

  mutable logging::logger m_logger;
  cache& m_cache;
  configuration_values_handler& m_values;
  surface_temperature_calculator& m_surface_temperature_calculator;
  data_types::values m_container;
  static const std::array<float, 6> m_values_substrate_type_one;
  static const std::array<float, 6> m_values_substrate_type_two;
};

}  // namespace mold::sprout

#endif
