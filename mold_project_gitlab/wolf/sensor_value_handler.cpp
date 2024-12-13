#include "sensor_value_handler.hpp"

#include "math_clamp.hpp"
#include "performance_profiler.hpp"
#include "sensor_id_enocean.hpp"
#include "sensor_value_type_helper.hpp"
#include "thread_checker.hpp"
#include "uuid_generator.hpp"
#include "wolf/timestamp_helper.hpp"

using namespace logging;
using namespace wolf;

sensor_value_handler::sensor_value_handler(
    boost::asio::io_service &service,
    sensor_value_offset_handler &offset_handler)
    : m_logger{"sensor_value_handler"},
      m_service(service),
      m_offset_handler(offset_handler) {
  m_offset_handler.signal_set.connect(
      [this](const auto &old, const auto &offset) {
        handle_offset_updated(old, offset);
      });
}

void sensor_value_handler::handle_list(
    const sensor_value_handler::values_type &values) {
  handle(values);
}

void sensor_value_handler::handle_single_value(const sensor_value &value) {
  handle({value});
}

namespace {
struct check_valid_datetime {
  logging::logger logger;
  bool operator()(const sensor_value &value) {
    // https://www.wolframalpha.com/input/?i=2016-01-01+UTC+to+unix+timestamp
    static const std::uint64_t time_stamp_2016_01_01 = 1451606400;
    const auto diff_since_epoch =
        wolf::timestamp_helper().cast_to_unix_seconds(value.timestamp);
    if (diff_since_epoch < time_stamp_2016_01_01) {
      LOGGING_LOG(logger, severity::warning)
          << "ignoring data because timestamp is before 2016-01-01 UTC,"
             " diff_since_epoch:"
          << std::dec << diff_since_epoch << " < " << time_stamp_2016_01_01;
      return false;
    }
    return true;
  }
};
struct offset_apply {
  sensor_value_offset_handler &offsets;
  sensor_value operator()(const sensor_value &value) {
    float value_with_offset =
        offsets.apply_offset_to_value(value.id, value.value);
    if (sensor_value_type_helper::is_humidity(value.id))
      value_with_offset = wolf::math::clamp<float>(value_with_offset, 0, 100);
    return sensor_value{value.id, value_with_offset, value.timestamp};
  }
};
}  // namespace

void sensor_value_handler::handle(const sensor_values &values) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle, values:" << values;
  wolf::thread_checker::throw_if_not_service("value_handler::handle");
  sensor_values with_valid_datetime;
  std::copy_if(values.cbegin(), values.cend(),
               std::back_inserter(with_valid_datetime),
               check_valid_datetime{m_logger});
  sensor_values with_offset;
  std::transform(with_valid_datetime.cbegin(), with_valid_datetime.cend(),
                 std::back_inserter(with_offset),
                 offset_apply{m_offset_handler});
  for (const auto &value : with_offset) save_value(value);
  if (with_offset.empty()) return;
  signal_values(with_offset);
}

const sensor_value_handler::entries_type &sensor_value_handler::get_all()
    const {
  return m_values;
}

boost::optional<float> sensor_value_handler::get_value_float(
    const wolf::sensor_id &id) const {
  const auto value = get_value(id);
  if (!value.has_value()) return boost::none;
  return value.get().value;
}

boost::optional<sensor_value> sensor_value_handler::get_value(
    const sensor_id &id) const {
  const auto found = find_sensor(id);
  if (found == m_values.end()) return boost::none;
  return *found;
}

void sensor_value_handler::save_value(const sensor_value &value) {
  LOGGING_LOG(m_logger, severity::normal) << "save, value:" << value;
  const auto found = find_sensor(value.id);
  if (found == m_values.cend())
    m_values.push_back(value);
  else
    *found = value;
}

sensor_value_handler::entries_type::iterator sensor_value_handler::find_sensor(
    const sensor_id &id) {
  return std::find_if(
      m_values.begin(), m_values.end(),
      [&id](const sensor_value &entry) { return entry.id == id; });
}

sensor_value_handler::entries_type::const_iterator
sensor_value_handler::find_sensor(const sensor_id &id) const {
  return std::find_if(
      m_values.cbegin(), m_values.cend(),
      [&id](const sensor_value &entry) { return entry.id == id; });
}

void sensor_value_handler::handle_offset_updated(
    const sensor_value_offset &old, const sensor_value_offset &offset) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_offset_updated, old:" << old << ", new:" << offset;

  const auto found = find_sensor(offset.id);
  if (found == m_values.cend()) return;
  const float value_without_offset = found->value - old.offset;
  handle_single_value({offset.id, value_without_offset, found->timestamp});
}
