#include "crc.hpp"
#include <array>
#include <boost/crc.hpp>

using namespace wolf::sensirion;

std::uint8_t crc::from_uint16(const std::uint16_t calculate) {
  std::array<std::uint8_t, 2> data;
  data[0] = (calculate >> 8) & 0xff;
  data[1] = calculate & 0xff;
  return from_carray(data.data(), data.size());
}

std::uint8_t crc::from_carray(const std::uint8_t *const data,
                              const std::size_t size) {
  const std::uint8_t crc =
      boost::crc<8, 0x31, 0xff, 0x00, false, false>(data, size);
  return crc;
}
