#include "profile_handler.hpp"

#include "wolf/enocean_link.hpp"
#include "wolf/profile_cache.hpp"
#include "wolf/sensor_type_helper.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/uuid_generator.hpp"

using namespace logging;
using namespace wolf;

profile_handler::profile_handler(profile_cache &profile_cache)
    : m_logger{"profile_handler"}, m_database(profile_cache) {
  load_from_database();
}

bool profile_handler::learn(const sensor_profile &profile) {
  LOGGING_LOG(m_logger, severity::verbose) << "learn, profile:" << profile;

  const auto found_it = find_profile(profile.id);
  const bool found = found_it != m_profiles.end();
  if (found) {
    if (*found_it == profile) {
      LOGGING_LOG(m_logger, severity::verbose)
          << "already learned profile:" << profile;
      return true;
    }
  }
  if (!learn_internal(
          profile))  // TODO dirty, adds profile internally to container.
  {
    return false;
  }
  LOGGING_LOG(m_logger, severity::normal)
      << "successfully learned profile:" << profile;
  if (found) {
    m_database.update(profile);
  } else {
    m_database.add(profile);
  }
  signal_update(profile);

  return true;
}

bool profile_handler::is_learned(const sensor_id &id) {
  return find_profile(id) != m_profiles.cend();
}

wolf::sensor_value_type profile_handler::get_sensor_value_type(
    const types::id &id, const types::id &owner) const {
  const auto found = std::find_if(
      m_profiles.cbegin(), m_profiles.cend(), [&](const auto &check) {
        return check.id.get_id() == id && check.id.get_owner() == owner;
      });
  if (found != m_profiles.cend()) return found->id.get_type();
  std::stringstream error;
  error << "could not find sensor by id/owner. id:" << id
        << ", owner:" << owner;
  throw std::runtime_error(error.str());
}

const profile_handler::learned &profile_handler::get_all() const {
  return m_profiles;
}

sensor_profile profile_handler::get(const sensor_id &id) const {
  const auto found = find_profile(id);
  if (found == m_profiles.cend())
    return {id, wolf::sensor_type_helper::get_type(id), {}};
  return *found;
}

void profile_handler::load_from_database() {
  const auto all_saved = m_database.get_all();
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "load_from_database, all_saved.size():" << all_saved.size();
  for (const auto &profile : all_saved) {
    if (!learn_internal(profile))
      LOGGING_LOG(m_logger, severity::error)
          << "load_from_database: profile not found";
  }
}

bool profile_handler::learn_internal(const sensor_profile &profile) {
  auto found = find_profile(profile.id);
  if (found != m_profiles.end())
    *found = profile;
  else
    m_profiles.push_back(profile);
  return true;
}

profile_handler::learned::const_iterator profile_handler::find_profile(
    const sensor_id &id) const {
  return std::find_if(
      m_profiles.cbegin(), m_profiles.cend(),
      [&id](const sensor_profile &item) { return item.id == id; });
}

profile_handler::learned::iterator profile_handler::find_profile(
    const sensor_id &id) {
  return std::find_if(
      m_profiles.begin(), m_profiles.end(),
      [&id](const sensor_profile &item) { return item.id == id; });
}
