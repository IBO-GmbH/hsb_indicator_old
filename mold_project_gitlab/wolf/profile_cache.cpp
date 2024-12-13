#include "profile_cache.hpp"

#include <boost/serialization/vector.hpp>
#include "wolf/boost_serialize_helper.hpp"
#include "wolf/boost_serialize_sensor_id.hpp"
#include "wolf/boost_serialize_sensor_profile.hpp"

using namespace wolf;

namespace soci {
template <>
struct type_conversion<sensor_profile> {
  using base_type = values;

  static void from_base(values const &values_, indicator /* ind */,
                        sensor_profile &to_cast) {
    const std::string details = values_.get<std::string>("details", "");
    if (details.empty()) {
      return;
    }
    wolf::serialize::decode_from_base64(details, to_cast);
  }

  static void to_base(const sensor_profile &to_cast, values &values_,
                      indicator &ind) {
    values_.set("sensor_id", to_cast.id.to_combined_id_base64());
    const std::string result_details =
        wolf::serialize::encode_to_base64(to_cast);
    values_.set("details", result_details);

    ind = i_ok;
  }
};
}  // namespace soci

profile_cache::profile_cache(wolf::database &database_)
    : m_logger{"profile_cache"},
      m_sync(m_logger, database_, "wolf_profile",
             {{"sensor_id", "text"}, {"details", "text"}}) {}

void profile_cache::add(const sensor_profile &to_add) { m_sync.add(to_add); }

void profile_cache::update(const sensor_profile &to_update) {
  m_sync.update(to_update);
}

void profile_cache::remove(const sensor_id &id) {
  m_sync.remove(id.to_combined_id_base64());
}

profile_cache::get_all_result profile_cache::get_all() {
  return m_sync.get_all();
}
