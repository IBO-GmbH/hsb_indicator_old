#include "configuration_state_handler.hpp"
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include "mold/boost_serialize_configuration.hpp"
#include "mold/boost_serialize_configuration_state.hpp"
#include "wolf/boost_serialize_helper.hpp"
#include "wolf/uuid_generator.hpp"

using namespace logging;
using namespace mold;

configuration_state_handler::configuration_state_handler(
    wolf::key_value_cache &key_value_database, const config config_)
    : m_logger("configuration_state_handler"),
      m_key_value_database(key_value_database),
      m_config{config_} {
  load_all();
}

const std::string database_key("configuration_state_handler");

void configuration_state_handler::save_all() {
  LOGGING_LOG(m_logger, severity::verbose) << "save_all()";
  const std::string to_save = wolf::serialize::encode_to_base64(m_states);
  m_key_value_database.set(database_key, to_save);
}

void configuration_state_handler::load_all() {
  LOGGING_LOG(m_logger, severity::verbose) << "load_all()";

  const auto saved = m_key_value_database.get(database_key);
  if (saved.empty()) {
    LOGGING_LOG(m_logger, severity::normal) << "nothing to load";
    return;
  }
  wolf::serialize::decode_from_base64(saved, m_states);
}

void configuration_state_handler::add(const configuration &to_add) {
  LOGGING_LOG(m_logger, severity::verbose) << "add, to_add:" << to_add;

  const auto found = find(to_add.id);
  if (found != m_states.end()) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not add config, because found != m_states.end(), id:"
        << to_add;
    throw std::runtime_error(
        "configuration_state_handler::add: ID already found!");
  }
  m_states.emplace_back(to_add, configuration_state{});
  save_all();
  signal_added(to_add.id, m_states.back().second);
}

void configuration_state_handler::update(const configuration &to_update) {
  LOGGING_LOG(m_logger, severity::verbose) << "update, to_update:" << to_update;

  auto found = find(to_update.id);
  if (found == m_states.cend()) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not update config, because found == m_states.end(), "
           "to_update:"
        << to_update;
    throw std::runtime_error(
        "configuration_state_handler::update: ID not found!");
  }
  found->first = to_update;
  save_all();
}

void configuration_state_handler::remove(const wolf::types::uuid_array &id) {
  LOGGING_LOG(m_logger, severity::verbose) << "remove, id:" << id;

  const auto found = find(id);
  if (found == m_states.end()) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not remove config, because found == m_states.end(), id:"
        << id;
    throw std::runtime_error(
        "configuration_state_handler::remove: ID not found!");
  }
  m_states.erase(found);
  save_all();
}

configuration_state_handler::all_result configuration_state_handler::get_all()
    const {
  all_result result;
  result.reserve(m_states.size());
  std::transform(m_states.cbegin(), m_states.cend(), std::back_inserter(result),
                 [](const container_entry &entry) {
                   return std::make_pair(entry.first.id, entry.second);
                 });
  return result;
}

std::optional<configuration_state>
configuration_state_handler::get_state_for_config(
    const configuration &configuration) {
  const auto found = find(configuration.id);
  if (found == m_states.cend()) return std::optional<configuration_state>();
  return found->second;
}

void configuration_state_handler::handle_mold_value(
    const mold_value &mold_value_) {
  if (mold_value_.sprout_type_ == sprout_type::infinite_days)
    handle_no_mold_value(mold_value_.configuration, mold_value_.timestamp);
  else
    handle_mold_value(mold_value_.configuration, mold_value_.percentage,
                      mold_value_.timestamp);
}

void configuration_state_handler::handle_confirm(
    const wolf::types::uuid_array &config_id) {
  LOGGING_LOG(m_logger, severity::normal)
      << "got confirmation, config_id:" << config_id;

  const auto found = find(config_id);
  if (found == m_states.cend()) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_confirm, could not find config! id:" << config_id;
    throw std::runtime_error(
        "configuration_state_handler::handle_confirm: ID not found!");
  }
  auto &state = found->second.state;
  if (state != calculation_state::yellow && state != calculation_state::red) {
    LOGGING_LOG(m_logger, severity::warning)
        << "handle_confirm, state neither yellow nor red, id:" << config_id
        << " configuration_state:" << found->second;
    throw std::runtime_error(
        "configuration_state_handler::handle_confirm: calculation_state "
        "neither yellow nor red");
  }
  const auto state_shall_be = [&] {
    if (state == calculation_state::yellow)
      return calculation_state::yellow_confirmed;
    return calculation_state::red_confirmed;
  }();
  state = state_shall_be;
  save_all();
  handle_state_change(found);
}

void configuration_state_handler::handle_no_mold_value(
    const wolf::types::uuid_array &id,
    const std::chrono::system_clock::time_point &now) {
  LOGGING_LOG(m_logger, severity::verbose) << "handle_no_mold_value, id:" << id;

  const auto found = find(id);
  if (found == m_states.end()) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_no_mold_value: could not find config, id:" << id;
    throw std::runtime_error(
        "configuration_state_handler::handle_no_mold_value: ID not found!");
  }

  const calculation_state current = found->second.state;
  const calculation_state should_be{calculation_state::green};
  if (current == should_be) {
    // everthing alright
    LOGGING_LOG(m_logger, severity::verbose)
        << "handle_no_mold_value: everthing alright, no change in "
           "calculation_state needed.";
    return;
  }

  found->second.state = should_be;
  found->second.time_since_green = now;

  save_all();
  handle_state_change(found);
}

static bool is_same_ignore_confirmed(const calculation_state first,
                                     const calculation_state second) {
  const auto is_same = [](const calculation_state left,
                          const calculation_state right) {
    if (left == calculation_state::yellow)
      return right == calculation_state::yellow ||
             right == calculation_state::yellow_confirmed;
    if (left == calculation_state::red)
      return right == calculation_state::red ||
             right == calculation_state::red_confirmed;
    return left == right;
  };
  return is_same(first, second) || is_same(second, first);
}

void configuration_state_handler::handle_mold_value(
    const wolf::types::uuid_array &config_id, const float mold_value,
    const std::chrono::system_clock::time_point &now) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "id:" << config_id << " value:" << mold_value;

  const auto found = find(config_id);
  if (found == m_states.cend()) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_mold_value: could not find config, id:" << config_id;
    throw std::runtime_error(
        "configuration_state_handler::handle_mold_value: ID not found!");
  }

  // if green -> yellow: yellow and beep
  // if yellow -> red: red and beep
  // if yellow_confirmed -> red: red and beep
  // if red_confirmed -> yellow: yellow_confirmed and no beep
  // if red -> yellow: yellow and no beep, but red is not confirmed, so the box
  // would beep all the time and there would be no change, so we do not have to
  // handle this case explicitly
  auto &current_state = found->second.state;
  const calculation_state should_be = [&] {
    if (mold_value > m_config.red_threshold) {
      return calculation_state::red;
    }
    if (mold_value > m_config.yellow_threshold) {
      if (current_state == calculation_state::red_confirmed)
        return calculation_state::yellow_confirmed;
      return calculation_state::yellow;
    }
    return calculation_state::green;
  }();
  if (is_same_ignore_confirmed(current_state, should_be)) return;
  if (should_be == calculation_state::green)
    found->second.time_since_green = now;
  else
    found->second.time_since_green.reset();
  current_state = should_be;
  save_all();
  handle_state_change(found);
}

configuration_state_handler::container::iterator
configuration_state_handler::find(const wolf::types::uuid_array &id) {
  return std::find_if(
      m_states.begin(), m_states.end(),
      [&id](const container_entry &entry) { return entry.first.id == id; });
}

void configuration_state_handler::handle_state_change(
    const container::iterator &changed) {
  LOGGING_LOG(m_logger, severity::normal)
      << "changing state to " << changed->second.state;
  signal_state_changed(changed->first.id, changed->second.state);
}
