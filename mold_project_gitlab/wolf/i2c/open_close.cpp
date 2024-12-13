#include "open_close.hpp"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include "log/logger.hpp"

using namespace wolf::i2c;

std::optional<device> open_close::open(const std::string &device_,
                                       const int slave_id) {
  static logging::logger logger("open_close");

  int result{};
  if ((result = ::open(device_.c_str(), O_RDWR)) < 0) {
    LOGGING_LOG(logger, logging::severity::verbose)
        << "failed to open device " << device_
        << " err:" << std::strerror(errno);
    return {};
  }
  if (::ioctl(result, I2C_SLAVE, slave_id) < 0) {
    LOGGING_LOG(logger, logging::severity::verbose)
        << "Failed to acquire bus access and/or talk to slave. device "
        << device_ << " err:" << std::strerror(errno) << " slave_id:0x"
        << std::hex << slave_id;
    return {};
  }
  return device{result};
}

void open_close::close(device &to_close) {
  if (to_close.get() != 0) ::close(to_close.get());
  to_close.get() = 0;
}
