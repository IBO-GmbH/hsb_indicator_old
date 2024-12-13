#include "login_handler.hpp"
#include "key_value_cache.hpp"
#include "uuid_generator.hpp"

using namespace logging;
using namespace wolf;

login_handler_implementation::login_handler_implementation(
    network::login::sender &network_login, key_value_cache &options,
    const device_information &information)
    : m_logger{"login_handler"},
      m_network_login(network_login),
      m_options{options},
      m_logged_in{false},
      m_device_information(information) {
  read_or_create_database_id();
}

void login_handler_implementation::handle_network_connected() {
  LOGGING_LOG(m_logger, severity::verbose) << "handle_network_connected()";
  const std::map<std::string, bool> capabilities{{"subs_type", true},
                                                 {"mold_sprout_value", true}};
  const network::login::sender::data to_send = {
      m_device_information.id, m_device_information.version, m_database_version,
      m_device_information.secret, capabilities};
  m_network_login.send(to_send,
                       [this](const auto &result) { handle_response(result); });
}

void login_handler_implementation::handle_network_closed() {
  LOGGING_LOG(m_logger, severity::normal)
      << "login_handler::handle_network_closed()";
  m_logged_in = false;
  signal_logged_in(m_logged_in);
}

bool login_handler_implementation::is_logged_in() const { return m_logged_in; }

void login_handler_implementation::handle_logged_in() {
  LOGGING_LOG(m_logger, severity::verbose)
      << "successfully handled response login";
  m_logged_in = true;
  signal_logged_in(m_logged_in);
}

void login_handler_implementation::handle_response(
    const std::optional<network::login::response::data> &response) {
  if (!response) {
    LOGGING_LOG(m_logger, severity::error) << "could not login";
    signal_login_error();
    return;
  }
  const auto &response_value = response.value();
  LOGGING_LOG(m_logger, severity::verbose)
      << "response_value.resync:" << response_value.resync;
  if (response_value.resync) {
    LOGGING_LOG(m_logger, severity::verbose) << "will resync";
    signal_resync();
  }
  handle_logged_in();
}

void login_handler_implementation::read_or_create_database_id() {
  const bool got_database_id = m_options.get_database_id(m_database_version);
  if (got_database_id) return;
  m_database_version = uuid_generator::generate_as_array();
  m_options.set_database_id(m_database_version);
}
