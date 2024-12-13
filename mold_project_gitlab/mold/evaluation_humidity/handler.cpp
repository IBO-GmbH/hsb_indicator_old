#include "handler.hpp"
#include "mold/sensor_id_evaluation_humidity.hpp"

using namespace mold::evaluation_humidity;

handler::handler(
    surface_temperature_calculator &surface_temperature_calculator_,
    configuration_values_handler &values_handler,
    const mold::config &program_options)
    : m_logger{"evaluation_humidity::handler"},
      m_surface_temperature_calculator(surface_temperature_calculator_),
      m_values_handler(values_handler),
      m_median_calculator_creator(program_options),
      m_filter_creator(
          {program_options.evaluation_humidity_.filter_constant,
           program_options.evaluation_humidity_.filter_threshold,
           program_options.evaluation_humidity_.filter_threshold_with_time,
           std::chrono::seconds(program_options.evaluation_humidity_
                                    .filter_threshold_timeout)}) {}

void handler::handle(const configuration_values &indoor,
                     const configuration_values_outdoor &outdoor) {
  BOOST_ASSERT_MSG(indoor.configuration_id != wolf::types::uuid_array{},
                   "invalid configuration id");
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle, indoor:" << indoor << ", outdoor:" << outdoor;
  if (!check_prerequists(indoor, outdoor)) return;
  const float indoor_temperature = indoor.indoor_temperature.value().value_;
  const float indoor_humidity = indoor.indoor_humidity.value().value_;
  const float outdoor_temperature = outdoor.temperature.value().value_;
  const float indoor_surface_temperature =
      m_surface_temperature_calculator.calculate(
          indoor.configuration_id, {indoor_temperature, outdoor_temperature});
  const float fraction_exponent =
      (indoor_temperature - indoor_surface_temperature) /
      ((243.12f + indoor_temperature) * (243.12f + indoor_surface_temperature));
  float evaluation_humidity =
      indoor_humidity * std::exp(4283.77f * fraction_exponent);
  evaluation_humidity = std::min(evaluation_humidity, 100.f);
  const auto now = get_timestamp(indoor, outdoor);
  log_timestamp(now, "evaluation humidity");
  const auto sensor_id =
      sensor_id_evaluation_humidity::create(indoor.configuration_id);
  auto &item = create_or_get(indoor.configuration_id);
  handle_median(item, sensor_id, evaluation_humidity, now);
  handle_filter(item, sensor_id, evaluation_humidity, now);
}

bool handler::check_prerequists(const configuration_values &indoor,
                                const configuration_values_outdoor &outdoor) {
  if (indoor.indoor_temperature && indoor.indoor_humidity &&
      outdoor.temperature)
    return true;
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "check_prerequists, failed. will skip evaluation_humidity "
         "calculation. indoor:"
      << indoor << ", outdoor:" << outdoor;
  return false;
}

handler::container::iterator handler::find_by_configuration(
    const wolf::types::uuid_array &configuration) {
  return std::find_if(
      m_container.begin(), m_container.end(),
      [&](const auto &item) { return item.configuration == configuration; });
}

handler::item &handler::create_or_get(
    const wolf::types::uuid_array &configuration) {
  auto found = find_by_configuration(configuration);
  if (found != m_container.cend()) return *found;
  m_container.push_back({configuration, m_median_calculator_creator.create(),
                         m_filter_creator.create()});
  return m_container.back();
}

void handler::handle_median(item &item, const wolf::sensor_id &sensor_id,
                            const float evaluation_humidity,
                            const std::chrono::system_clock::time_point &now) {
  const auto evaluation_humidity_median =
      item.median_calculator.calculate_median(evaluation_humidity, now);
  const auto sensor_value =
      wolf::sensor_value{sensor_id, evaluation_humidity_median, now};
  signal_median(item.configuration, sensor_value);
}

void handler::handle_filter(item &item, const wolf::sensor_id &sensor_id,
                            const float evaluation_humidity,
                            const std::chrono::system_clock::time_point &now) {
  float evaluation_humidity_filtered = evaluation_humidity;
  if (!m_filter_handler.filter_value(wolf::sensor_type::evaluation_humidity,
                                     item.filter, evaluation_humidity_filtered))
    return;
  const auto sensor_value =
      wolf::sensor_value{sensor_id, evaluation_humidity_filtered, now};
  log_timestamp(now, "evaluation humidity filtered value");
  signal_filtered(sensor_value);
}

std::chrono::system_clock::time_point handler::get_timestamp(
    const mold::configuration_values &indoor,
    const mold::configuration_values_outdoor &outdoor) const {
  log_timestamp(indoor.indoor_temperature->timestamp_, "indoor temperature");
  log_timestamp(indoor.indoor_humidity->timestamp_, "indoor humidity");
  log_timestamp(outdoor.temperature->timestamp_, "outdoor temperature");
  return std::max(std::max(indoor.indoor_temperature->timestamp_,
                           indoor.indoor_humidity->timestamp_),
                  outdoor.temperature->timestamp_);
}

void handler::log_timestamp(
    const std::chrono::system_clock::time_point &timestamp,
    const std::string &name) const {
  auto time = std::chrono::system_clock::to_time_t(timestamp);
  std::tm *ptm = std::localtime(&time);
  char buffer[32];
  std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << name << " timestamp: " << buffer;
}

void handler::handle_indoor(const configuration_values &values) {
  const auto outdoor = m_values_handler.get_last_outdoor_value();
  handle(values, outdoor);
}

void handler::handle_outdoor(const configuration_values_outdoor &values) {
  const auto all = m_values_handler.get_all();
  for (const auto &indoor : all) handle(indoor, values);
}
