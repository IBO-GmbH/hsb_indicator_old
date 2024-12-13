#include "mold_value_handler.hpp"
#include <numeric>
#include "sprout_percentage_calculator.hpp"
#include "wolf/math_compare_decimals.hpp"

using namespace mold;

mold_value_handler::mold_value_handler(sprout::handler& sprout_,
                                       const mold::config& program_options)
    : m_logger{"mold_value_handler"},
      m_sprout(sprout_),
      m_filter_creator(
          {program_options.mold_value_.filter_constant,
           program_options.mold_value_.filter_threshold,
           program_options.mold_value_.filter_threshold_with_time,
           std::chrono::seconds(
               program_options.mold_value_.filter_threshold_timeout)}) {}

void mold_value_handler::recalculate(const wolf::types::uuid_array& config_id,
                                     const time_point& now) {
  const auto value = m_sprout.get(config_id);
  calculate_and_save(value, now);
}

const mold_value_handler::values& mold_value_handler::get_all() const {
  return m_values;
}

void mold_value_handler::calculate_and_save(
    const sprout::data_types::value& value, const time_point& now) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "calculate_and_save, configuration:" << value.configuration;
  mold_value value_ = calculate(value, now);
  auto found = find(value.configuration);
  if (found != m_values.end())
    *found = value_;
  else
    m_values.push_back(value_);
  float mold_value_filtered = value_.percentage;
  auto& filter = create_or_get(value.configuration);
  if (!m_filter_handler.filter_value(wolf::sensor_type::sprout_value, filter,
                                     mold_value_filtered))
    return;
  value_.percentage = mold_value_filtered;
  signal_value(value_);
}

namespace {
struct calculate_value {
  using timestamp = std::chrono::duration<double>;
  const sprout::data_types::value& value;
  const std::chrono::system_clock::time_point& now;
  const sprout_type& sprout_type_fall;
  float operator()(const sprout_type type) {
    const auto type_as_index = static_cast<std::size_t>(type);
    const auto timestamps = value.timestamps_[type_as_index];
    if (type <= sprout_type_fall && value.type_ != sprout_type::infinite_days)
      return sprout_percentage_calculator::rising(timestamps.start.get(), now,
                                                  type);
    return sprout_percentage_calculator::falling(timestamps, now, type);
  }
};
}  // namespace

static std::array<float, 5> calculate_values(
    const sprout::data_types::value& value,
    const std::chrono::system_clock::time_point& now) {
  static const std::array types = {
      sprout_type::sixteen_days, sprout_type::eight_days,
      sprout_type::four_days, sprout_type::two_days, sprout_type::one_day};
  std::array<float, 5> result;
  const auto type_fall =
      sprout_percentage_calculator::get_type_falling(value.timestamps_);
  std::transform(types.cbegin(), types.cend(), result.begin(),
                 calculate_value{value, now, type_fall});
  return result;
}

mold_value mold_value_handler::calculate(const sprout::data_types::value& from,
                                         const time_point& now) const {
  mold_value result;
  result.configuration = from.configuration;
  const auto timestamp_now = std::chrono::system_clock::now();
  const auto now_to_use = timestamp_now > now ? timestamp_now : now;
  result.timestamp = now_to_use;
  const auto percentages = calculate_values(from, now_to_use);
  const auto max_position =
      std::max_element(percentages.cbegin(), percentages.cend());
  const auto percentage = *max_position;
  const auto type_diff = max_position - percentages.cbegin() + 1;
  auto type = static_cast<sprout_type>(type_diff);
  if (wolf::math::compare_floats(percentage, 0.f))
    type = static_cast<sprout_type>(sprout_type::infinite_days);
  std::stringstream log_string;
  log_string << "percentage:" << percentage << ", percentages:";
  std::copy(percentages.cbegin(), percentages.cend(),
            std::ostream_iterator<float>(log_string, ", "));
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << log_string.str() << "type: " << type;
  result.percentage = percentage;
  result.sprout_type_ = type;
  return result;
}

mold_value_handler::values::iterator mold_value_handler::find(
    const wolf::types::uuid_array& id) {
  return std::find_if(m_values.begin(), m_values.end(), [&](const auto& item) {
    return item.configuration == id;
  });
}

wolf::low_pass_filter& mold_value_handler::create_or_get(
    const wolf::types::uuid_array& configuration) {
  auto found = std::find_if(
      m_filters.begin(), m_filters.end(),
      [&](const auto& filter) { return filter.first == configuration; });
  if (found != m_filters.end())
    return found->second;
  else
    m_filters.push_back({configuration, m_filter_creator.create()});
  return m_filters.back().second;
}
