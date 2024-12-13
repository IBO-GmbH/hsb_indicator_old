#include "sensor_profile.hpp"
#include <algorithm>
#include "wolf/sensor_metadata_creator.hpp"

wolf::sensor_id wolf::actor::sensor_profile::create_sensor_id(
    const types::id_esp3 &actor_esp3_id) {
  types::id id(5);
  id[0] = static_cast<std::uint8_t>(sensor_type::actor_switch);
  std::copy_n(reinterpret_cast<const std::uint8_t *>(&actor_esp3_id), 4,
              std::next(id.begin(), 1));
  return sensor_id(id, sensor_value_type::actor_switch);
}

void wolf::actor::sensor_profile::add_actor_switch_to_room(
    const wolf::actor::actor &actor_, const wolf::types::id_esp3 &base_id,
    wolf::room &room_, wolf::room_handler &room_handler,
    wolf::profile_handler &profile_handler) {
  const auto profile = create_sensor_profile(actor_, base_id);
  const auto found = std::find_if(room_.sensors.cbegin(), room_.sensors.cend(),
                                  [&profile](const wolf::sensor_id &id) {
                                    return id.get_id() == profile.id.get_id();
                                  });
  if (found != room_.sensors.cend()) return;
  profile_handler.learn(profile);
  room_.sensors.push_back(profile.id);
  room_handler.update(room_);
}

wolf::sensor_profile wolf::actor::sensor_profile::create_sensor_profile(
    const wolf::actor::actor &actor_, const types::id_esp3 &base_id) {
  const auto actor_esp3_id = actor_.id + base_id;
  const auto id = create_sensor_id(actor_esp3_id);
  const auto metadata =
      wolf::sensor_metadata_creator::actor(actor_.id, actor_esp3_id);
  return wolf::sensor_profile({id, sensor_type::actor_switch, metadata});
}
