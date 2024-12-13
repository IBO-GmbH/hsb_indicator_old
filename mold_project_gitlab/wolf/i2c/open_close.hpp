#ifndef WOLF_I2C_OPENER_HPP
#define WOLF_I2C_OPENER_HPP

#include <optional>
#include <string>
#include "device.hpp"

namespace wolf {
namespace i2c {

class open_close {
 public:
  static std::optional<device> open(const std::string &device_,
                                    const int slave_id);
  static void close(device &to_close);
};

}  // namespace i2c
}  // namespace wolf

#endif  // WOLF_I2C_OPENER_HPP
