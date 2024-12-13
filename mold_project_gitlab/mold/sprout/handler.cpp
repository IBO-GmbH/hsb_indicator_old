#include "handler.hpp"
#include "mold/sprout/cache.hpp"
#include "sprout_percentage_calculator.hpp"

using namespace mold::sprout;

const std::array<float, 6> handler::m_values_substrate_type_one = {
    0.75f, 0.77f, 0.795f, 0.82f, 0.85f, 0.9f};
const std::array<float, 6> handler::m_values_substrate_type_two = {
    0.79f, 0.80f, 0.835f, 0.86f, 0.89f, 0.94f};

handler::handler(
    cache &cache_, mold::configuration_values_handler &values,
    surface_temperature_calculator &surface_temperature_calculator_)
    : m_logger{"sprout::handler"},
      m_cache(cache_),
      m_values(values),
      m_surface_temperature_calculator(surface_temperature_calculator_) {
  load_all();
}

void handler::add_configuration(const wolf::types::uuid_array &id) {
  BOOST_ASSERT_MSG(find_by_configuration(id) == m_container.cend(),
                   "configuration already in container");
  m_container.emplace_back();
  auto &added = m_container.back();
  added.configuration = id;
  added.type_ = sprout_type::infinite_days;
  const auto now = std::chrono::system_clock::now();
  for (auto &timestamps : added.timestamps_) {
    timestamps.start = now;
    timestamps.change = now;
  }
  save_all();
}

void handler::remove_configuration(const wolf::types::uuid_array &id) {
  auto found = find_by_configuration(id);
  BOOST_ASSERT_MSG(found != m_container.cend(),
                   "configuration not in container");
  if (found == m_container.cend()) return;
  m_container.erase(found);
  save_all();
}

void handler::handle_evaluation_humidity(const wolf::types::uuid_array &id,
                                         const wolf::sensor_value &value,
                                         const substrate_type &type) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle_evaluation_humidity, id: " << id << ", value: " << value
      << ", substrate_type: " << type;
  const auto configuration_values = m_values.get_or_throw(id);
  const auto outdoor = m_values.get_last_outdoor_value();
  calculate(id, value, configuration_values, outdoor, type);
}

const data_types::values &handler::get_all() const { return m_container; }

static float max_value(const float surface_temperature,
                       const float timespan_constant) {
  const float division = (surface_temperature - 30) / 7.5f;
  const float buffer =
      (0.009f * std::cosh(division) + timespan_constant - 0.009f) * 100;
  return buffer;
}

void handler::calculate(
    const wolf::types::uuid_array &id,
    const wolf::sensor_value &evaluation_humidity,
    const mold::configuration_values &values,
    const configuration_values_handler::outdoor_values &outdoor,
    const substrate_type &type) {
  const auto border_points_ = border_points(id, values, outdoor, type);
  calculate(id, border_points_, evaluation_humidity);
}

void handler::calculate(const wolf::types::uuid_array &id,
                        const handler::border_points_type &border_points_,
                        const wolf::sensor_value &evaluation_humidity) {
  auto &value = get(id);
  const auto now = evaluation_humidity.timestamp;
  const auto evaluation_humidity_value = evaluation_humidity.value;
  const auto type_ =
      type_from_border_points(border_points_, evaluation_humidity_value);
  if (value.type_ == type_) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "sprout type didn't change, type_:" << type_;
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "change to type:" << type_;
  if (!handle_initial_change(value, now)) {
    handle_curve_change_timestamps(value, type_, now);
  }
  value.type_ = type_;
  save_all();
  signal_sprout_type_changed(value);
}

mold::sprout_type handler::type_from_border_points(
    const handler::border_points_type &border_points_,
    const float evaluation_humidity) const {
  std::array to_iterate = {
      mold::sprout_type::one_day, mold::sprout_type::two_days,
      mold::sprout_type::four_days, mold::sprout_type::eight_days,
      mold::sprout_type::sixteen_days};
  for (const auto type : to_iterate) {
    const auto border_point = border_points_[static_cast<std::size_t>(type)];
    if (evaluation_humidity >= border_point) return type;
  }
  return mold::sprout_type::infinite_days;
}

handler::border_points_type handler::border_points(
    const wolf::types::uuid_array &id, const mold::configuration_values &values,
    const mold::configuration_values_handler::outdoor_values &outdoor,
    const substrate_type &type) {
  BOOST_ASSERT_MSG(outdoor.temperature, "outdoor.temperature not set");
  BOOST_ASSERT_MSG(values.indoor_temperature,
                   "values.indoor_temperature not set");
  const float outdoor_temperature = outdoor.temperature.value().value_;
  const float indoor_temperature = values.indoor_temperature.value().value_;
  const surface_temperature_calculator::temperatures surface_temperatures{
      indoor_temperature, outdoor_temperature};
  const auto surface_temperature =
      m_surface_temperature_calculator.calculate(id, surface_temperatures);
  /* Humidity Borders: for different timespans there are different max.
   values for humidity. Last array element is for one day, second to last for
   two days, third to last for 4 days, 8 days, 16 days and the first one is
   for an infinite amount of time*/
  std::array<float, 6> humidity_borders;
  std::array<float, 6> calculation_factors;
  calculation_factors = type == substrate_type::type_one
                            ? m_values_substrate_type_one
                            : m_values_substrate_type_two;
  std::transform(calculation_factors.cbegin(), calculation_factors.cend(),
                 humidity_borders.begin(), [&](const auto calculation_factor) {
                   return max_value(surface_temperature, calculation_factor);
                 });
  return humidity_borders;
}

data_types::values::iterator handler::find_by_configuration(
    const wolf::types::uuid_array &configuration) {
  return std::find_if(
      m_container.begin(), m_container.end(),
      [&](const auto &item) { return item.configuration == configuration; });
}

data_types::value &handler::get(const wolf::types::uuid_array &id) {
  auto found = find_by_configuration(id);
  if (found != m_container.cend()) return *found;
  std::stringstream debug;
  debug << "could not find the value by id:" << id;
  throw std::runtime_error(debug.str());
}

void handler::load_all() { m_container = m_cache.load_all(); }

void handler::save_all() { m_cache.save_all(m_container); }

bool handler::handle_initial_change(data_types::value &value,
                                    const time_point &now) {
  auto &timestamps_ = value.timestamps_;
  if (value.type_ == sprout_type::infinite_days &&
      sprout_percentage_calculator::all_equal(timestamps_)) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "we come from initial infinite and are now in higher curve, set all "
           "to first timestamp";
    for (std::size_t index = 0; index < timestamps_.size(); ++index) {
      timestamps_[index].start = now;
      timestamps_[index].change = now;
    }
    return true;
  }
  return false;
}

void handler::handle_curve_change_timestamps(data_types::value &value,
                                             const sprout_type &type,
                                             const handler::time_point &now) {
  const auto type_as_index = static_cast<std::size_t>(type);
  const auto type_as_index_value = static_cast<std::size_t>(value.type_);
  auto &timestamps_ = value.timestamps_;
  for (std::size_t index = type_as_index_value + 1; index <= type_as_index;
       ++index) {
    const auto index_type = static_cast<sprout_type>(index);
    const auto new_start = sprout_percentage_calculator::new_start(
        timestamps_[index], now, index_type);
    timestamps_[index].start = new_start;
  }
  if (type_as_index < type_as_index_value)
    for (std::size_t index = type_as_index; index <= type_as_index_value;
         ++index)
      timestamps_[index].change = now;
  else
    timestamps_[type_as_index].change = now;
}
