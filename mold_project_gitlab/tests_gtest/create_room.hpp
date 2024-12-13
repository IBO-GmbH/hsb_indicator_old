#ifndef CREATE_ROOM_HPP
#define CREATE_ROOM_HPP

#include "mold/room_creator.hpp"
#include "wolf/sensor_id_shtc1.hpp"

template <class injector_type>
static auto create_room(injector_type& injector,
                        std::vector<wolf::sensor_id> indoor) {
  auto room_creator = injector.template create<mold::room_creator>();
  return room_creator.create(wolf::types::uuid_array{}, "name", indoor, wolf::types::uuid_array{}, false);
}

template <class injector_type>
static auto create_room(injector_type& injector) {
  wolf::sensor_id sensor_id_indoor_temperature =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::temperature);
  wolf::sensor_id sensor_id_indoor_humidity =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::humidity);
  return create_room(injector,
                     {sensor_id_indoor_temperature, sensor_id_indoor_humidity});
}

#endif
