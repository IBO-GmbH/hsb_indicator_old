#ifndef WOLF_SENSIRION_CRC_HPP
#define WOLF_SENSIRION_CRC_HPP

#include <cstdint>

namespace wolf {
namespace sensirion {

class crc {
 public:
  static std::uint8_t from_uint16(const std::uint16_t calculate);
  static std::uint8_t from_carray(const uint8_t* const data,
                                  const std::size_t size);
};

}  // namespace sensirion
}  // namespace wolf

#endif  // WOLF_SENSIRION_CRC_HPP
