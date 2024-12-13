#include "configuration_cache.hpp"

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include "mold/boost_serialize_configuration.hpp"
#include "mold/configuration.hpp"
#include "wolf/base64.hpp"
#include "wolf/boost_serialize_helper.hpp"
#include "wolf/thread_checker.hpp"

using namespace mold;
using namespace logging;

namespace soci {
template <>
struct type_conversion<configuration> {
  using base_type = values;

  static void from_base(values const &values_, indicator /* ind */,
                        configuration &to_cast) {
    wolf::types::id id =
        wolf::base64::decode(values_.get<std::string>("config_uuid"));
    if (id.size() != 16) {
      throw std::runtime_error(
          "configuration_cache form_base size of id != 16!");
    }
    std::copy(id.cbegin(), id.cend(), to_cast.id.begin());
    const std::string details_encoded = values_.get<std::string>("details", "");
    // TODO write in sync table
    if (details_encoded.empty()) {
      return;
    }
    wolf::serialize::decode_from_base64(details_encoded, to_cast);
  }

  static void to_base(const configuration &to_cast, values &values_,
                      indicator &ind) {
    values_.set("config_uuid", wolf::base64::encode(to_cast.id));

    const std::string result_details =
        wolf::serialize::encode_to_base64(to_cast);
    values_.set("details", result_details);

    ind = i_ok;
  }
};
}  // namespace soci

configuration_cache::configuration_cache(wolf::database &database_)
    : m_logger{"configuration_cache"},
      m_sync(m_logger, database_, "mold_configuration",
             {{"config_uuid", "text"}, {"details", "text"}}) {}

void configuration_cache::add(const configuration &to_add) {
  LOGGING_LOG(m_logger, severity::verbose) << "adding, to_add:" << to_add;
  wolf::thread_checker::throw_if_not_service("configuration_cache::add");
  m_sync.add(to_add);
}

void configuration_cache::update(const configuration &to_update) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "update, to_update:" << to_update;
  wolf::thread_checker::throw_if_not_service("configuration_cache::update");
  m_sync.update(to_update);
}

void configuration_cache::remove(const wolf::types::uuid_array &id) {
  LOGGING_LOG(m_logger, severity::verbose) << "remove, id:" << id;
  wolf::thread_checker::throw_if_not_service("configuration_cache::remove");
  const std::string id_casted = wolf::base64::encode(id);
  m_sync.remove(id_casted);
}

configuration_cache::configurations configuration_cache::get_all() {
  return m_sync.get_all();
}
