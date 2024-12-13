#ifndef WOLF_I2C_READ_WRITE_HPP
#define WOLF_I2C_READ_WRITE_HPP

#include "device.hpp"
#include "log/logger.hpp"

namespace wolf {
namespace i2c {

class read_write {
 public:
  read_write(logging::logger& logger, const device& file);

  template <typename container_type>
  bool write_buffer(container_type& read_into) {
    return write_buffer(read_into.data(), read_into.size());
  }
  bool write_byte(char send);
  template <typename container_type>
  bool read_into_buffer(container_type& read_into) {
    return read_into_buffer(read_into.data(), read_into.size());
  }
  bool read_byte(char& read_into);

 private:
  bool write_buffer(std::uint8_t* const data, std::size_t size);
  bool read_into_buffer(std::uint8_t* const data, std::size_t size);

 private:
  logging::logger& m_logger;
  const device& m_file;
};
}  // namespace i2c
}  // namespace wolf

#endif  // WOLF_I2C_READ_WRITE_HPP
