#ifndef WOLF_ESP3_CRC_H
#define WOLF_ESP3_CRC_H

#include "wolf/types.hpp"

#include <algorithm>

namespace wolf {
namespace esp3_crc {
namespace implementation {
std::uint8_t crc8(std::uint8_t crc, std::uint8_t data);
}

template <class iterator_type>
std::uint8_t compute_crc8(iterator_type first, iterator_type last) {
  std::uint8_t result{};
  std::for_each(first, last, [&](const std::uint8_t& work) {
    result = implementation::crc8(result, work);
  });
  return result;
}

std::uint8_t compute_crc8(const types::data& from);
}
}

#endif  // WOLF_ESP3_CRC_H
