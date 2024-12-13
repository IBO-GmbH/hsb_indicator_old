#include "cache.hpp"
#include <boost/serialization/optional.hpp>
#include <boost/serialization/vector.hpp>
#include "wolf/boost_serialize_chrono_system_clock_timepoint.hpp"
#include "wolf/boost_serialize_helper.hpp"

using namespace mold::sprout;

static const std::string database_key("sprout_cache");

cache::cache(wolf::key_value_cache &key_value_database)
    : m_logger{"sprout::cache"}, m_key_value_database(key_value_database) {}

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, data_types::timestamp &to_serialize,
               const unsigned int /*version*/) {
  archive &to_serialize.start;
  archive &to_serialize.change;
}
template <class Archive>
void serialize(Archive &archive, data_types::value &to_serialize,
               const unsigned int /*version*/) {
  archive &to_serialize.configuration;
  archive &to_serialize.type_;
  archive &to_serialize.timestamps_;
}
}  // namespace boost::serialization

void cache::save_all(const data_types::values &values) {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "save_all()";
  const std::string to_save = wolf::serialize::encode_to_base64(values);
  m_key_value_database.set(database_key, to_save);
}

data_types::values cache::load_all() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "load_all()";
  const auto saved = m_key_value_database.get(database_key);
  if (saved.empty()) {
    LOGGING_LOG(m_logger, logging::severity::normal) << "nothing to load";
    return {};
  }
  data_types::values result;
  wolf::serialize::decode_from_base64(saved, result);
  return result;
}
