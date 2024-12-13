#include "configuration.hpp"

#include <boost/optional/optional_io.hpp>
#include "wolf/uuid_generator.hpp"

using namespace mold;

configuration::configuration(const wolf::types::uuid_array &configuration_uuid_,
                             const wolf::types::uuid_array &room_uuid)
    : id(configuration_uuid_), room(room_uuid) {}

configuration::configuration(const wolf::types::uuid_array &room_id,
                             const wolf::sensor_id &id_temperature_indoor_,
                             const wolf::sensor_id &id_humidity_indoor_)
    : configuration(wolf::uuid_generator::generate_as_array(), room_id,
                    id_temperature_indoor_, id_humidity_indoor_) {}

configuration::configuration(const wolf::types::uuid_array &configuration_uuid_,
                             const wolf::types::uuid_array &room_uuid,
                             const wolf::sensor_id &id_temperature_indoor_,
                             const wolf::sensor_id &id_humidity_indoor_)
    : id(configuration_uuid_),
      room(room_uuid),
      temperature(id_temperature_indoor_),
      humidity(id_humidity_indoor_) {}

std::ostream &mold::operator<<(std::ostream &out,
                               const configuration &to_print) {
  out << "{id:" << to_print.id << " room:" << to_print.room
      << " temperature:" << to_print.temperature
      << " humidity:" << to_print.humidity
      << " mold_settings:" << to_print.mold_settings_ << "}";
  return out;
}

bool mold::configuration::operator==(const mold::configuration &compare) const {
  return compare.humidity == humidity && compare.id == id &&
         compare.mold_settings_ == mold_settings_ && compare.room == room &&
         compare.temperature == temperature;
}

bool configuration::operator!=(const configuration &compare) const {
  return !(*this == compare);
}
