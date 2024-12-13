#ifndef MOLD_CONFIGURATION
#define MOLD_CONFIGURATION

#include <boost/optional.hpp>
#include "mold/mold_settings.hpp"
#include "wolf/sensor_id.hpp"
#include "wolf/types.hpp"

namespace mold {

struct configuration {
  configuration() = default;
  explicit configuration(const wolf::types::uuid_array& configuration_uuid_,
                         const wolf::types::uuid_array& room_uuid);
  explicit configuration(const wolf::types::uuid_array& room_id,
                         const wolf::sensor_id& id_temperature_indoor_,
                         const wolf::sensor_id& id_humidity_indoor_);
  explicit configuration(const wolf::types::uuid_array& configuration_uuid_,
                         const wolf::types::uuid_array& room_uuid,
                         const wolf::sensor_id& id_temperature_indoor_,
                         const wolf::sensor_id& id_humidity_indoor_);
  bool operator==(const configuration& compare) const;
  bool operator!=(const configuration& compare) const;

  wolf::types::uuid_array id{};
  wolf::types::uuid_array room;

  wolf::sensor_id temperature;
  wolf::sensor_id humidity;

  boost::optional<mold_settings> mold_settings_;
};

std::ostream& operator<<(std::ostream& out, const configuration& to_print);
}  // namespace mold

#endif  // MOLD_CONFIGURATION
