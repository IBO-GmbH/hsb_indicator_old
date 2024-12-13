#ifndef MOLD_EVALUATION_HUMIDITY_HANDLER_HPP
#define MOLD_EVALUATION_HUMIDITY_HANDLER_HPP

#include "mold/configuration_values_handler.hpp"
#include "mold/median_calculator_creator.hpp"
#include "mold/program_options_parser.hpp"
#include "mold/surface_temperature_calculator.hpp"
#include "wolf/low_pass_filter_creator.hpp"
#include "wolf/sensor_filter.hpp"

namespace mold::evaluation_humidity {

class handler {
 public:
  handler(surface_temperature_calculator& surface_temperature_calculator_,
          configuration_values_handler& values_handler,
          const mold::config& program_options);

  void handle_indoor(const configuration_values& values);
  void handle_outdoor(const configuration_values_outdoor& values);
  void handle(const configuration_values& indoor,
              const configuration_values_outdoor& outdoor);
  boost::signals2::signal<void(const wolf::sensor_value&)> signal_filtered;
  boost::signals2::signal<void(const wolf::types::uuid_array&,
                               const wolf::sensor_value&)>
      signal_median;

 private:
  struct item {
    wolf::types::uuid_array configuration;
    mold::median_calculator median_calculator;
    wolf::low_pass_filter filter;
  };
  using container = std::vector<item>;
  bool check_prerequists(const configuration_values& indoor,
                         const configuration_values_outdoor& outdoor);
  container::iterator find_by_configuration(
      const wolf::types::uuid_array& configuration);
  item& create_or_get(const wolf::types::uuid_array& configuration);
  void handle_median(handler::item& item, const wolf::sensor_id& sensor_id,
                     const float evaluation_humidity,
                     const std::chrono::system_clock::time_point& now);
  void handle_filter(handler::item& item, const wolf::sensor_id& sensor_id,
                     const float evaluation_humidity,
                     const std::chrono::system_clock::time_point& now);
  std::chrono::system_clock::time_point get_timestamp(
      const mold::configuration_values& indoor,
      const mold::configuration_values_outdoor& outdoor) const;
  void log_timestamp(const std::chrono::system_clock::time_point& timestamp,
                     const std::string& name) const;

  mutable logging::logger m_logger;
  surface_temperature_calculator& m_surface_temperature_calculator;
  configuration_values_handler& m_values_handler;
  mold::median_calculator_creator m_median_calculator_creator;
  wolf::low_pass_filter_creator m_filter_creator;
  container m_container;
  wolf::sensor_filter m_filter_handler;
};
}  // namespace mold::evaluation_humidity

#endif
