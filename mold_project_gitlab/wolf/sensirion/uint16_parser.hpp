#ifndef WOLF_SENSIRION_UINT16_PARSER_HPP
#define WOLF_SENSIRION_UINT16_PARSER_HPP
#include <array>
#include <optional>

namespace wolf {
namespace sensirion {

class uint16_parser {
 public:
  using data_type = std::array<std::uint8_t, 3>;
  static std::optional<std::uint16_t> check_crc8_and_cast(
      const data_type& todo);

 private:
  static bool check_crc8(const data_type::const_iterator& todo);
  static std::uint16_t cast(const data_type::const_iterator& todo);
};

}  // namespace sensirion
}  // namespace wolf

#endif  // WOLF_SENSIRION_UINT16_PARSER_HPP
