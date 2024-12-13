#include "sprout_percentage_calculator.hpp"
#include "log/logger.hpp"

float mold::sprout_percentage_calculator::rising(
    const time_point &start,
    const mold::sprout_percentage_calculator::time_point &now,
    const sprout_type &type) {
  return get_percentage(start, now, type);
}

float mold::sprout_percentage_calculator::falling(
    const mold::sprout::data_types::timestamp &timestamps,
    const mold::sprout_percentage_calculator::time_point &now,
    const sprout_type &type) {
  const auto percentage_rise =
      rising(timestamps.start.get(), timestamps.change.get(), type);
  const auto percentage_fall =
      percentage_rise - get_percentage(timestamps.change.get(), now, type);
  return static_cast<float>(std::max(.0f, percentage_fall));
}

mold::sprout_percentage_calculator::time_point
mold::sprout_percentage_calculator::new_start(
    const mold::sprout::data_types::timestamp &timestamps,
    const mold::sprout_percentage_calculator::time_point &now,
    const sprout_type &type) {
  const auto percentage = falling(timestamps, now, type);
  const auto limit = get_limit(type);
  using timestamp = std::chrono::duration<int64_t>;
  return now - std::chrono::duration_cast<timestamp>(percentage * limit);
}

mold::sprout_type mold::sprout_percentage_calculator::get_type_falling(
    const mold::sprout::data_types::timestamps &timestamps) {
  const auto max_position = std::max_element(
      timestamps.cbegin(), timestamps.cend(),
      [](const auto &timestamp_one, const auto &timestamp_two) {
        return timestamp_one.change.get().time_since_epoch().count() <
               timestamp_two.change.get().time_since_epoch().count();
      });
  const auto type_diff = max_position - timestamps.cbegin();
  auto type = static_cast<sprout_type>(type_diff);
  // if sprout value was initial, mold value can only rise, not fall
  if (type == sprout_type::infinite_days && all_equal(timestamps))
    type = sprout_type::one_day;
  return type;
}

bool mold::sprout_percentage_calculator::all_equal(
    const mold::sprout::data_types::timestamps &timestamps) {
  return all_start_equal(timestamps) && all_change_equal(timestamps);
}

float mold::sprout_percentage_calculator::get_percentage(
    const mold::sprout_percentage_calculator::time_point &start,
    const mold::sprout_percentage_calculator::time_point &now,
    const sprout_type &type) {
  const auto diff = now - start;
  const auto limit = get_limit(type);
  using timestamp = std::chrono::duration<double>;
  const auto percentage = std::chrono::duration_cast<timestamp>(diff) /
                          std::chrono::duration_cast<timestamp>(limit);
  return static_cast<float>(std::min(1.0, percentage));
}

std::chrono::hours mold::sprout_percentage_calculator::get_limit(
    const sprout_type &type) {
  const auto one_day = std::chrono::hours(24);
  switch (type) {
    case sprout_type::one_day:
      return one_day;
    case sprout_type::two_days:
      return one_day * 2;
    case sprout_type::four_days:
      return one_day * 4;
    case sprout_type::eight_days:
      return one_day * 8;
    case sprout_type::sixteen_days:
      return one_day * 16;
    default:
      BOOST_ASSERT_MSG(false, "must not reach");
      break;
  }
  return one_day * 16;
}

bool mold::sprout_percentage_calculator::all_start_equal(
    const sprout::data_types::timestamps &timestamps) {
  const auto start_check = timestamps.front().start;
  return std::all_of(timestamps.cbegin(), timestamps.cend(),
                     [&start_check](const auto &timestamps_element) {
                       return timestamps_element.start == start_check;
                     });
}

bool mold::sprout_percentage_calculator::all_change_equal(
    const mold::sprout::data_types::timestamps &timestamps) {
  const auto change_check = timestamps.front().change;
  return std::all_of(timestamps.cbegin(), timestamps.cend(),
                     [&change_check](const auto &timestamps_element) {
                       return timestamps_element.change == change_check;
                     });
}
