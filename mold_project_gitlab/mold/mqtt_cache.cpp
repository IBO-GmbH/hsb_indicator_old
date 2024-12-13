#include "mqtt_cache.hpp"
#include "wolf/base64.hpp"

namespace soci {
template <>
struct type_conversion<mold::mqtt_cache::mqtt_client> {
  using base_type = values;

  static void from_base(values const &values_, indicator /* ind */,
                        mold::mqtt_cache::mqtt_client &to_cast) {
    to_cast.client_id = values_.get<std::string>("client_id", "");
    wolf::types::id room_id =
        wolf::base64::decode(values_.get<std::string>("room_uuid"));
    if (room_id.size() != 16)
      throw std::runtime_error(
          "type_conversion<mqtt_client>::from_base: id not 16 signs");
    std::copy(room_id.cbegin(), room_id.cend(), to_cast.room_id.begin());
  }

  static void to_base(const mold::mqtt_cache::mqtt_client &to_cast,
                      values &values_, indicator &ind) {
    values_.set("client_id", to_cast.client_id);
    values_.set("room_uuid", wolf::base64::encode(to_cast.room_id));

    ind = i_ok;
  }
};
}  // namespace soci

mold::mqtt_cache::mqtt_cache(wolf::database &database_)
    : m_logger{"mqtt_cache"},
      m_sync(m_logger, database_, "mqtt_client",
             {{"client_id", "text"}, {"room_uuid", "text"}}) {}

void mold::mqtt_cache::add(const mold::mqtt_cache::mqtt_client &to_add) {
  m_sync.add(to_add);
}

void mold::mqtt_cache::remove(const std::string &to_remove) {
  m_sync.remove(to_remove);
}

mold::mqtt_cache::clients mold::mqtt_cache::get_all() {
  return m_sync.get_all();
}
