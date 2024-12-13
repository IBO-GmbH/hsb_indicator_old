#include "message.hpp"
#include "wolf/network/bond_deserializer.hpp"
#include "wolf/uuid_helper.hpp"

using namespace wolf::network::response;

message::message(bond_parser &source, boost::asio::io_service &service,
                 const std::chrono::steady_clock::duration &response_timeout)
    : m_logger{"network::response::message"},
      m_response_timeout{response_timeout},
      m_timeout(service) {
  m_connection_message = source.signal_message.connect(
      [this](const auto &message) { return handle_with_catch(message); });
}

void message::register_work_uuid(const types::uuid_array &work_id,
                                 const message::callback &to_call) {
  const auto to_add = std::make_pair(work_id, to_call);
  m_waiting_for_response.push_back(to_add);
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "m_response_timeout:" << std::dec
      << std::chrono::duration_cast<std::chrono::seconds>(m_response_timeout)
             .count()
      << ", adding work_id:" << work_id;
  if (m_waiting_for_response.size() > 20) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "there are more than 20 responses waiting, count:" << std::dec
        << m_waiting_for_response.size();
  }
  if (m_response_timeout.count() <= 0 || m_timeout_active) return;

  m_last_work_id = work_id;
  boost::system::error_code error;
  m_timeout.expires_from_now(m_response_timeout, error);
  if (error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not call expires_from_now on timer, message:"
        << error.message();
    return;
  }
  m_timeout_active = true;
  m_timeout.async_wait([this](const boost::system::error_code &error) {
    handle_timeout(error);
  });
}

void message::clear() {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "clearing m_waiting_for_response, size:"
      << m_waiting_for_response.size();
  for (const auto &timed_out : m_waiting_for_response)
    timed_out.second(std::optional<Clima::Message>());
  m_waiting_for_response.clear();
  if (m_response_timeout.count() == 0 || !m_timeout_active) return;
  m_timeout_active = false;
  boost::system::error_code error_code;
  m_timeout.cancel(error_code);
  if (error_code)
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "clear: could not cancel timer, message:" << error_code.message();
}

std::size_t message::get_waiting() const {
  return m_waiting_for_response.size();
}

bool message::handle_with_catch(const Clima::Message &message) {
  try {
    return handle(message);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "could not handle message, error:" << error.what();
  }
  return false;
}

bool message::handle(const Clima::Message &message) {
  if (message.Type != ::Clima::Enums::MessageType::Response) return false;
  bond_deserializer deserializer;
  const Clima::Response response =
      deserializer.parse_message<Clima::Response>(message.Entry);
  uuid_helper uuid_helper_;
  const auto response_uuid =
      uuid_helper_.vector_to_array(response.ConfirmationUUID);

  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle_response(), current work-items waiting for a response:"
      << std::dec << m_waiting_for_response.size() << " code:" << std::dec
      << " response_uuid:" << response_uuid;

  if (m_response_timeout.count() > 0 && m_timeout_active) {
    if (m_last_work_id == response_uuid) {
      boost::system::error_code error;
      m_timeout.cancel(error);
      if (error)
        LOGGING_LOG(m_logger, logging::severity::warning)
            << "could not cancel timer, message:" << error.message();
    }
  }

  const auto it_response = std::find_if(
      m_waiting_for_response.cbegin(), m_waiting_for_response.cend(),
      [&](const auto &item) { return item.first == response_uuid; });
  if (it_response == m_waiting_for_response.cend()) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "it_response == m_waiting_for_response.cend() invalid "
           "response-uuid."
           " MUST NOT HAPPEN, id:"
        << response_uuid;
    return false;
  }
  const auto to_call = it_response->second;
  // WATCHOUT: to_call may invalidate it_response!
  m_waiting_for_response.erase(it_response);
  to_call(message);
  return true;
}
void message::handle_timeout(const boost::system::error_code &error) {
  m_timeout_active = false;
  if (error) {
    if (error.value() == boost::asio::error::operation_aborted)
      // timer got cancelled --> ignore
      return;
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "handle_timeout, error:" << error.message();
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::warning)
      << "response timed out, m_last_work_id:" << m_last_work_id;
  // we got a timeout!
  signal_timeout();
}
