#include "send_helper.hpp"

using namespace wolf::network::response;

send_helper::send_helper(sender &sender_,
                         const wolf::types::uuid_array &work_uuid)
    : m_logger{"network::response::send_helper"},
      m_sender(sender_),
      m_work_uuid(work_uuid) {}

void send_helper::success() { m_sender.send_success(m_work_uuid); }

void send_helper::failure(const std::string &description) {
  m_sender.send_failure(m_work_uuid, description);
}
