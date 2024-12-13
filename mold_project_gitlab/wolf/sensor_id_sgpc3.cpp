#include "sensor_id_sgpc3.hpp"
#include <cassert>

using namespace wolf;

sensor_id sensor_id_sgpc3::create(const sensor_sgpc3_channel &sgpc3_channel) {
  types::id id(2);
  id.front() = static_cast<std::uint8_t>(sensor_type::sgpc3);
  id[1] = static_cast<std::uint8_t>(sgpc3_channel);
  const auto sensor_value_type = [&] {
    switch (sgpc3_channel) {
      case sensor_sgpc3_channel::tvoc:
        return sensor_value_type::voc;
    }
    assert(false);
    return sensor_value_type::unknown;
  }();
  return sensor_id{id, sensor_value_type};
}
