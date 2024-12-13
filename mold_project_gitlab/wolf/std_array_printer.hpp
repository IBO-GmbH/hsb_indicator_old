#ifndef STD_ARRAY_PRINTER_HPP
#define STD_ARRAY_PRINTER_HPP

#include <array>
#include <string>

namespace wolf {
class std_array_printer {
 public:
  template <std::size_t Count>
  static std::string to_string(const std::array<std::uint8_t, Count>& write) {
    return to_string(write.data(), write.size());
  }

 private:
  static std::string to_string(const std::uint8_t* write,
                               const std::size_t size);
};
}  // namespace wolf

#endif  // STD_ARRAY_PRINTER_HPP
