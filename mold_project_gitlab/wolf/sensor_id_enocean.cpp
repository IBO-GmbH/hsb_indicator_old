#include "sensor_id_enocean.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "enocean_channel_helper.hpp"

using namespace wolf;

sensor_id sensor_id_enocean::create(
    const types::id_esp3 &esp3_id, const CHANNEL_TYPE &channel_,
    const std::optional<enocean_subchannel_type> &subchannel,
    const types::eep &eep) {
  types::id id(9);
  id[0] = static_cast<std::uint8_t>(sensor_type::enocean);
  std::copy_n(reinterpret_cast<const std::uint8_t *>(&esp3_id), 4,
              std::next(id.begin(), 1));
  if (subchannel.has_value()) {
    std::copy_n(reinterpret_cast<const std::uint8_t *>(&channel_), 3,
                std::next(id.begin(), 5));
    const auto type_with_subchannel =
        enocean_channel_helper::get_type_with_subchannel(channel_);
    // if subchannel has value type_with_subchannel does have a value too
    const std::uint8_t subchannel_offset =
        3 * static_cast<std::uint8_t>(type_with_subchannel.value());
    id[8] = static_cast<std::uint8_t>(subchannel.value()) - subchannel_offset;
  } else
    std::copy_n(reinterpret_cast<const std::uint8_t *>(&channel_), 4,
                std::next(id.begin(), 5));
  enocean_channel_helper helper;
  const auto sensor_type = helper.cast_to_wolf(channel_, subchannel, eep);
  return sensor_id(id, sensor_type);
}

types::id_esp3 sensor_id_enocean::convert_to_esp3_id(
    const sensor_id &to_convert) {
  if (to_convert.get_id().empty()) return {};
  types::id_esp3 id;
  const auto sensor = to_convert.get_id();
  id = static_cast<types::id_esp3>(sensor[1] | (sensor[2] << 8) |
                                   (sensor[3] << 16) | (sensor[4] << 24));
  return id;
}

std::string sensor_id_enocean::esp3_id_string(const types::id_esp3 &id) {
  std::stringstream stream;
  stream << "0x" << std::hex << id;
  return stream.str();
}

std::string sensor_id_enocean::eep_string(const types::eep &eep) {
  std::stringstream stream;
  stream << std::hex << +eep[0] << "-" << std::setw(2) << std::setfill('0')
         << +eep[1] << "-" << std::setw(2) << std::setfill('0') << +eep[2];
  return stream.str();
}
