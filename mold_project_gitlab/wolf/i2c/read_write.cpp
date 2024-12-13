#include "read_write.hpp"

using namespace wolf::i2c;
using namespace logging;

read_write::read_write(logger& logger, const device& file)
    : m_logger(logger), m_file(file) {}

bool read_write::write_byte(char send) {
  auto written = write(m_file.get(), &send, 1);
  if (written == 1) return true;
  LOGGING_LOG(m_logger, severity::error)
      << "could not write byte! to m_file:" << std::dec << m_file.get()
      << " send:" << std::hex << static_cast<int>(send);
  return false;
}

bool read_write::read_into_buffer(std::uint8_t* const data, std::size_t size) {
  const ssize_t read_result = read(m_file.get(), data, size);
  if (read_result != static_cast<ssize_t>(size)) {
    LOGGING_LOG(m_logger, severity::error)
        << "Failed to read on i2c device, read_result != size, "
           "err:"
        << strerror(errno) << ", size:" << std::dec << size
        << ", read_result:" << read_result;
    return false;
  }
  return true;
}

bool read_write::read_byte(char& read_into) {
  const ssize_t read_result = read(m_file.get(), &read_into, 1);
  if (read_result <= 0) {
    LOGGING_LOG(m_logger, severity::error)
        << "Failed to read byte on i2c device err:" << strerror(errno);
    return false;
  }
  return true;
}

bool read_write::write_buffer(uint8_t* const data, std::size_t size) {
  std::size_t written =
      static_cast<std::size_t>(write(m_file.get(), data, size));
  if (written == size) return true;
  LOGGING_LOG(m_logger, severity::error)
      << "could not write byte! to m_file:" << std::dec << m_file.get()
      << " size:" << std::dec << size;
  return false;
}
