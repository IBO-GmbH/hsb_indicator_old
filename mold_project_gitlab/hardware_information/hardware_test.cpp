#include "log/log.hpp"
#include "log/logger.hpp"
#include "wolf/hardware_address.hpp"

using namespace logging;

int main() {
  initialize();
  logging::logger main_logger{"main"};

  LOGGING_LOG(main_logger, severity::verbose) << "starting up";

  LOGGING_LOG(main_logger, severity::normal)
      << "wolf::hardware_address::get_serial_number_as_hex():"
      << wolf::hardware_address::get_hostname_as_hex();
}
