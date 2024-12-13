#include "uint16_parser.hpp"
#include <sstream>
#include <stdexcept>
#include "crc.hpp"

using namespace wolf::sensirion;

std::optional<std::uint16_t> uint16_parser::check_crc8_and_cast(
    const data_type &todo) {
  if (!check_crc8(todo.cbegin())) return {};
  return cast(todo.cbegin());
}

bool uint16_parser::check_crc8(const data_type::const_iterator &todo) {
  const std::uint8_t *data = &(*todo);
  const std::uint8_t crc = crc::from_carray(data, 2);
  return (crc == *std::next(todo, 2));
}

uint16_t uint16_parser::cast(const data_type::const_iterator &todo) {
  auto result = static_cast<std::uint16_t>(*todo) << 8;
  result |= *(std::next(todo));
  return static_cast<std::uint16_t>(result);
}
