#include "configuration_values_handler.hpp"
#include "wolf/math_clamp.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_value_type_helper.hpp"
#include "wolf/thread_checker.hpp"

using namespace logging;
using namespace mold;

configuration_values_handler::configuration_values_handler()
    : m_logger("configuration_values_handler") {}

void configuration_values_handler::add(const configuration &configuration_) {
  LOGGING_LOG(m_logger, severity::verbose) << "add:" << configuration_;

  const auto found = find(configuration_.id);
  if (found != m_values.cend()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "config already added, configuration_:" << configuration_;
    throw std::runtime_error(
        "configuration_values_handler::add: ID already found!");
  }
  m_values.emplace_back(configuration_,
                        configuration_values{configuration_.id, {}, {}});
  signal_value(m_values.back().second);
}

void configuration_values_handler::update(const configuration &configuration_) {
  LOGGING_LOG(m_logger, severity::verbose) << "update:" << configuration_;

  const auto found = find(configuration_.id);
  if (found == m_values.cend()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "config not found for update, configuration_:" << configuration_;
    throw std::runtime_error(
        "configuration_values_handler::update: ID not found!");
  }
  found->first = configuration_;
}

void configuration_values_handler::remove(const wolf::types::uuid_array &id) {
  LOGGING_LOG(m_logger, severity::verbose) << "remove:" << id;

  const auto found = find(id);
  if (found == m_values.cend()) {
    LOGGING_LOG(m_logger, severity::warning) << "config not found, id:" << id;
    throw std::runtime_error(
        "configuration_values_handler::remove: ID not found!");
  }
  m_values.erase(found);
}

configuration_values_handler::all_result configuration_values_handler::get_all()
    const {
  all_result result;
  result.reserve(m_values.size());
  std::transform(m_values.cbegin(), m_values.cend(), std::back_inserter(result),
                 [](const container_entry &entry) { return entry.second; });
  return result;
}

std::optional<configuration_values> configuration_values_handler::get(
    const wolf::types::uuid_array &id) const {
  const auto found = find(id);
  if (found == m_values.cend()) return {};
  return found->second;
}

configuration_values configuration_values_handler::get_or_throw(
    const wolf::types::uuid_array &id) const {
  const auto result = get(id);
  if (result) return *result;
  std::stringstream error;
  error << "could not find configuration_values by id:" << id;
  throw std::runtime_error(error.str());
}

const configuration_values_handler::outdoor_values &
configuration_values_handler::get_last_outdoor_value() const {
  return m_last_outdoor_values;
}

void configuration_values_handler::set_outdoor_sensor(
    const wolf::outdoor_sensor &id) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "setting outdoor sensor, id:" << id;
  if (m_outdoor_sensor == id) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "set_outdoor_sensor, same sensor id, won't trigger signals. id:"
        << id;
    return;
  }
  m_outdoor_sensor = id;
}

static bool has_temperature_or_humidity(const wolf::sensor_values &values) {
  for (const auto &value : values)
    if (wolf::sensor_value_type_helper::is_temperature(value.id) ||
        wolf::sensor_value_type_helper::is_humidity(value.id))
      return true;
  return false;
}

static bool set_value_to_container_item(
    const wolf::sensor_value &value,
    configuration_values_handler::container_entry &entry) {
  const configuration_values::value value_container = {value.timestamp,
                                                       value.value};
  if (value.id == entry.first.temperature) {
    entry.second.indoor_temperature = value_container;
    return true;
  }
  if (value.id == entry.first.humidity) {
    entry.second.indoor_humidity = value_container;
    return true;
  }
  return false;
}

void configuration_values_handler::handle_values(
    const wolf::sensor_values &values) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_value, values: " << values;
  if (values.empty()) return;
  if (!has_temperature_or_humidity(values)) return;

  const std::vector<configuration_values> affected_values =
      handle_indoor(values);
  for (const auto &affected_value : affected_values) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "indoor sensors did change, affected_value:" << affected_value;
    signal_value(affected_value);
  }
  const bool changed_outdoor = handle_outdoor(values);
  if (changed_outdoor) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "outdoor sensor values did change, m_last_outdoor_values:"
        << m_last_outdoor_values;
    signal_values_outdoor(m_last_outdoor_values);
  }
}

namespace {
struct by_configuration_id {
  wolf::types::uuid_array configuration;
  bool operator()(const configuration_values &values) {
    return values.configuration_id == configuration;
  }
};
}  // namespace

std::vector<configuration_values> configuration_values_handler::handle_indoor(
    const wolf::sensor_values &values) {
  std::vector<configuration_values> affected_values;
  for (auto &item : m_values) {
    BOOST_ASSERT_MSG(item.first.id != wolf::types::uuid_array{},
                     "invalid config id");
    std::optional<configuration_values> affected;
    for (const auto &value : values) {
      if (!set_value_to_container_item(value, item)) continue;
      affected = item.second;
    }
    if (affected) affected_values.push_back(affected.value());
  }
  return affected_values;
}

bool configuration_values_handler::handle_outdoor(
    const wolf::sensor_values &values) {
  bool set{};
  for (const auto &value : values) {
    const configuration_values::value value_container = {value.timestamp,
                                                         value.value};
    if (value.id == m_outdoor_sensor.temperature) {
      m_last_outdoor_values.temperature = value_container;
      set = true;
      continue;
    }
    if (value.id == m_outdoor_sensor.humidity) {
      m_last_outdoor_values.humidity = value_container;
      set = true;
    }
  }
  return set;
}

void configuration_values_handler::reset_outdoor_humidity_value() {
  m_last_outdoor_values.humidity.reset();
  signal_values_outdoor(m_last_outdoor_values);
}

configuration_values_handler::container::iterator
configuration_values_handler::find(const wolf::types::uuid_array &id) {
  return std::find_if(
      m_values.begin(), m_values.end(),
      [&](const container_entry &entry) { return entry.first.id == id; });
}

configuration_values_handler::container::const_iterator
configuration_values_handler::find(const wolf::types::uuid_array &id) const {
  return std::find_if(
      m_values.cbegin(), m_values.cend(),
      [&](const container_entry &entry) { return entry.first.id == id; });
}

bool configuration_values_handler::sensor_is_outdoor(
    const wolf::types::id_esp3 &id) const {
  if (!m_outdoor_sensor.is_set()) return false;
  auto is_outdoor = false;
  if (m_outdoor_sensor.temperature.is_set()) {
    auto temperature_id = wolf::sensor_id_enocean::convert_to_esp3_id(
        m_outdoor_sensor.temperature);
    is_outdoor = temperature_id == id;
  }
  if (m_outdoor_sensor.humidity.is_set()) {
    auto humidity_id =
        wolf::sensor_id_enocean::convert_to_esp3_id(m_outdoor_sensor.humidity);
    is_outdoor = humidity_id == id;
  }
  return is_outdoor;
}
