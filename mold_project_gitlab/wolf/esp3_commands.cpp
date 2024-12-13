#include "esp3_commands.hpp"
#include <sstream>
#include "Includes/eoSerialCommand.h"
#include "esp3_sender.hpp"

using namespace logging;
using namespace wolf;

esp3_commands::esp3_commands(esp3_sender &sender)
    : m_logger{"esp3_commands"},
      m_sender(sender),
      m_waiting_for{waiting_for::nothing} {}

void esp3_commands::get_base_id() {
  const std::vector<std::uint8_t> data{CO_RD_IDBASE};
  m_sender.send(data, PACKET_COMMON_COMMAND, 1, 0);

  m_waiting_for = waiting_for::base_id;
}

void esp3_commands::handle_response(const types::data &data,
                                    const types::data & /*optional_data*/) {
  std::stringstream bytes;
  for (const auto &byte : data)
    bytes << " " << std::hex << static_cast<int>(byte);

  bytes.flush();

  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_response, data.size():" << data.size()
      << " data:" << bytes.str();

  if (m_waiting_for != waiting_for::base_id) {
    return;
  }
  if (data[0] != 0) {
    LOGGING_LOG(m_logger, severity::error) << "getting base_id failed!";
    return;
  }
  const types::id_esp3 id = static_cast<const types::id_esp3>(
      data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4]);
  signal_base_id(id);

  m_waiting_for = waiting_for::nothing;
}
