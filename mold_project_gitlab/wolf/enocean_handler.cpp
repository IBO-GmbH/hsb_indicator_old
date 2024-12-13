#include "enocean_handler.hpp"
#include "wolf/enocean_link.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_metadata_creator.hpp"

using namespace wolf;

enocean_handler::enocean_handler(wolf::enocean_cache &cache)
    : m_logger{"enocean_handler"},
      m_cache(cache),
      m_enocean_link{std::make_unique<enocean_link>()} {
  load();
}

enocean_handler::~enocean_handler() = default;

namespace {
struct cast_profile {
  const types::id_esp3 id;
  const types::eep eep;
  sensor_profile operator()(
      const enocean_link::register_result &to_cast) const {
    const auto id_casted =
        sensor_id_enocean::create(id, to_cast.type, to_cast.subchannel, eep);
    auto metadata = sensor_metadata_creator::enocean(to_cast.range_from,
                                                     to_cast.range_to, eep, id);
    const sensor_profile result{id_casted, sensor_type::enocean, metadata};
    return result;
  }
};
}  // namespace

static std::vector<sensor_profile> cast_profiles(
    enocean_link::register_results &to_cast, const types::id_esp3 id,
    const types::eep eep) {
  std::vector<sensor_profile> result;
  std::transform(to_cast.cbegin(), to_cast.cend(), std::back_inserter(result),
                 cast_profile{id, eep});
  return result;
}

std::vector<sensor_profile> enocean_handler::register_(const types::id_esp3 id,
                                                       const types::eep eep,
                                                       const bool is_outdoor) {
  m_cache.add({id, eep});
  enocean_link::register_results channels =
      m_enocean_link->register_device(id, eep[0], eep[1], eep[2], is_outdoor);
  return cast_profiles(channels, id, eep);
}

void enocean_handler::handle_value(const types::id_esp3 id,
                                   const types::data &data, const int rssi,
                                   const bool is_outdoor) {
  const auto now = std::chrono::system_clock::now();
  handle_value(id, data, rssi, now, is_outdoor);
}

namespace {
struct cast_value {
  const types::id_esp3 id;
  const std::chrono::system_clock::time_point timestamp;
  const types::eep eep;
  sensor_value operator()(const enocean_link::parse_result &value) const {
    const auto id_casted =
        sensor_id_enocean::create(id, value.type, value.subchannel, eep);
    const sensor_value result{id_casted, value.value, timestamp};
    return result;
  }
};
}  // namespace

static std::vector<sensor_value> cast_values(
    const enocean_link::parse_results &to_cast, const types::id_esp3 id,
    const std::chrono::system_clock::time_point &timestamp,
    const types::eep &eep) {
  std::vector<sensor_value> result;
  std::transform(to_cast.cbegin(), to_cast.cend(), std::back_inserter(result),
                 cast_value{id, timestamp, eep});
  return result;
}

void enocean_handler::handle_value(
    const types::id_esp3 id, const types::data &data, const int rssi,
    const std::chrono::system_clock::time_point &timestamp,
    const bool is_outdoor) {
  const auto parsed = m_enocean_link->parse_telegram(id, data, is_outdoor);
  if (parsed.empty()) return;
  const auto eep = m_enocean_link->get_eep_of_device(id);
  if (!eep.has_value())
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "tried to handle value of unregistered device id, id: 0x" << std::hex
        << id;
  const auto values = cast_values(parsed, id, timestamp, eep.value());
  signal_values(values, rssi);
}

void enocean_handler::remove_devices(const std::vector<types::id_esp3> &ids) {
  m_enocean_link->remove_devices(ids);
  m_cache.remove_devices(ids);
}

void enocean_handler::load() {
  const auto saved = m_cache.get_all();
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "load, saved:" << saved.size();
  for (const auto &item : saved) {
    auto loaded_profiles = m_enocean_link->register_device(
        item.id, item.eep[0], item.eep[1], item.eep[2], false);
    signal_profiles_loaded(cast_profiles(loaded_profiles, item.id, item.eep));
  }
}
