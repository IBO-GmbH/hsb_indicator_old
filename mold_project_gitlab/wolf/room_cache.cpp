#include "room_cache.hpp"

#include "wolf/boost_serialize_helper.hpp"
#include "wolf/boost_serialize_sensor_id.hpp"
#include "wolf/room.hpp"

#include <boost/serialization/vector.hpp>

using namespace wolf;

namespace soci {
template <>
struct type_conversion<room> {
  using base_type = values;

  static void from_base(values const &values_, indicator /* ind */,
                        room &to_cast) {
    types::id id = base64::decode(values_.get<std::string>("room_uuid"));
    if (id.size() != 16)
      throw std::runtime_error(
          "type_conversion<room>::from_base: id not 16 signs");
    std::copy(id.cbegin(), id.cend(), to_cast.id.begin());
    to_cast.name = values_.get<std::string>("name", "");
    const std::string sensors_encoded = values_.get<std::string>("sensors", "");
    if (sensors_encoded.empty()) {
      return;
    }
    serialize::decode_from_base64(sensors_encoded, to_cast.sensors);
  }

  static void to_base(const room &to_cast, values &values_, indicator &ind) {
    values_.set("room_uuid", base64::encode(to_cast.id));
    values_.set("name", to_cast.name);
    const std::string result_sensors =
        serialize::encode_to_base64(to_cast.sensors);
    values_.set("sensors", result_sensors);

    ind = i_ok;
  }
};
}  // namespace soci

room_cache::room_cache(database &database_)
    : m_logger{"room_cache"},
      m_sync(m_logger, database_, "wolf_room",
             {{"room_uuid", "text"}, {"name", "text"}, {"sensors", "text"}}) {}

void room_cache::add(const room &to_add) { m_sync.add(to_add); }

void room_cache::update(const room &to_update) { m_sync.update(to_update); }

void room_cache::remove(const types::uuid_array &to_remove) {
  const std::string id_casted = base64::encode(to_remove);
  m_sync.remove(id_casted);
}

std::vector<room> room_cache::get_all() { return m_sync.get_all(); }
