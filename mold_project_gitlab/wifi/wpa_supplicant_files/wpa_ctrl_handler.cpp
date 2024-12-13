#include "wpa_ctrl_handler.hpp"

#define MAX_REPLY 4096

wifi::wpa_ctrl_handler::wpa_ctrl_handler() : m_logger("wpa_ctrl_handler") {}

bool wifi::wpa_ctrl_handler::check_if_open(wpa_ctrl *ctrl) const {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "check if interface is open";
  std::string reply;
  const auto command_return = handle_command(ctrl, "PING", reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, did not open correctly!";
    return false;
  }
  if (reply.find("PONG") == std::string::npos) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "wpa_supplicant did not answer correctly to ping!";
    return false;
  }
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "control interface successfully opened!";
  return true;
}

void wifi::wpa_ctrl_handler::list_all_available_networks(wpa_ctrl *ctrl) const {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "list all available networks";
  std::string reply;
  auto command_return = handle_command(ctrl, "SCAN", reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not scan for networks!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose) << reply;
  command_return = handle_command(ctrl, "SCAN_RESULTS", reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not get scan results!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::normal) << reply;
}

void wifi::wpa_ctrl_handler::list_all_connected_networks(wpa_ctrl *ctrl) const {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "list all already configured networks";
  const auto networks = get_all_connected_networks(ctrl);
  if (networks.has_value())
    LOGGING_LOG(m_logger, logging::severity::normal) << networks.value();
}

void wifi::wpa_ctrl_handler::connect_to_network(
    wpa_ctrl *ctrl, const std::string &name,
    const std::function<std::string()> &password_callback) const {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "connect to network: " << name;
  const auto configured = get_single_network_from_list(ctrl, name);
  optional_string network_id;
  if (!configured.has_value())
    add_new_network(ctrl, name, network_id, password_callback);
  std::string reply;
  if (!network_id.has_value()) {
    network_id = get_id_from_network_line(configured.value());
    if (!network_id.has_value()) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "could not get id of the network!";
      return;
    }
  }
  const auto command_return =
      handle_command(ctrl, ("SELECT_NETWORK " + network_id.value()), reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not select new network!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose) << reply;
}

void wifi::wpa_ctrl_handler::display_current_network(wpa_ctrl *ctrl) const {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "get current network";
  const auto current_network = get_single_network_from_list(ctrl, "CURRENT");
  if (!current_network.has_value()) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "there is no current network!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "current network: " << current_network.value();
}

void wifi::wpa_ctrl_handler::remove_network(wpa_ctrl *ctrl,
                                            const std::string &name) const {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "remove network: " << name;
  std::string id = name;
  if (!is_id(id)) {
    const auto network = get_single_network_from_list(ctrl, name);
    if (!network.has_value()) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "there is no configured network with this name!";
      return;
    }
    const auto network_id = get_id_from_network_line(network.value());
    if (!network_id.has_value()) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "could not get id of the network!";
      return;
    }
    id = network_id.value();
  }
  std::string reply;
  auto command_return = handle_command(ctrl, ("REMOVE_NETWORK " + name), reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not remove network!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose) << reply;
}

void wifi::wpa_ctrl_handler::save_configuartion(wpa_ctrl *ctrl) const {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "save current configuration";
  std::string reply;
  const auto command_return = handle_command(ctrl, "SAVE_CONFIG", reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not save config!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose) << reply;
}

bool wifi::wpa_ctrl_handler::handle_command(wpa_ctrl *ctrl,
                                            const std::string &command,
                                            std::string &reply) const {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "sending command: " << command;
  char reply_data[MAX_REPLY] = "";
  size_t reply_size = MAX_REPLY;
  const auto return_value = wpa_ctrl_request(
      ctrl, command.data(), command.length(), reply_data, &reply_size, nullptr);
  if (return_value == -1) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "Error while sending command or receiving reply!";
    return false;
  }
  if (return_value == -2) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "Timeout while sending command!";
    return false;
  }
  reply = std::string(reply_data);
  return true;
}

wifi::wpa_ctrl_handler::optional_string
wifi::wpa_ctrl_handler::get_all_connected_networks(wpa_ctrl *ctrl) const {
  std::string reply;
  const auto command_return = handle_command(ctrl, "LIST_NETWORKS", reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not get networks!";
    return optional_string();
  }
  return optional_string(reply);
}

wifi::wpa_ctrl_handler::optional_string
wifi::wpa_ctrl_handler::get_single_network_from_list(
    wpa_ctrl *ctrl, const std::string &to_search) const {
  auto all_networks = get_all_connected_networks(ctrl);
  if (!all_networks.has_value()) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "there are no configured networks!";
    return optional_string();
  }
  const auto position_found = all_networks->find(to_search);
  if (position_found == std::string::npos) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "could not find search string in network list, search string: "
        << to_search;
    return optional_string();
  }
  const char new_line = '\n';
  const auto new_end = all_networks->find(new_line, position_found);
  if (new_end == std::string::npos) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "there is no new_line after the search string, allthough "
           "there always should be any!";
    return optional_string();
  }
  all_networks->erase(new_end);
  const auto new_beginning = all_networks->rfind(new_line, position_found);
  if (new_beginning == std::string::npos) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "there is no new_line before the search string, allthough "
           "there always should be any!";
    return optional_string();
  }
  all_networks->erase(0, new_beginning + 1);
  return all_networks;
}

wifi::wpa_ctrl_handler::optional_string
wifi::wpa_ctrl_handler::get_id_from_network_line(
    const std::string &network) const {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "get id of network: " << network;
  const char id_char = network.front();
  if (!isdigit(id_char)) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "first character is not a number, but should be network id!";
    return optional_string();
  }
  std::string id;
  id.push_back(id_char);
  return optional_string(id);
}

bool wifi::wpa_ctrl_handler::is_id(const std::string &to_check) {
  if (to_check.length() > 1) return false;
  if (!std::isdigit(*to_check.cbegin())) return false;
  return true;
}

void wifi::wpa_ctrl_handler::add_new_network(
    wpa_ctrl *ctrl, const std::string &name,
    wifi::wpa_ctrl_handler::optional_string &network_id_select,
    const std::function<std::string()> &password_callback) const {
  std::string password = password_callback();
  std::string reply;
  auto command_return = handle_command(ctrl, "ADD_NETWORK", reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not add new network!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "index of new network: " << reply.front();
  std::string network_id;
  network_id.push_back(reply.front());
  command_return = handle_command(
      ctrl, ("SET_NETWORK " + network_id + " ssid \"" + name + "\""), reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not set ssid of new network!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose) << reply;
  command_return = handle_command(
      ctrl, ("SET_NETWORK " + network_id + " psk \"" + password + "\""), reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not set wpa passphrase "
           "of new network!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose) << reply;
  command_return =
      handle_command(ctrl, ("ENABLE_NETWORK " + network_id), reply);
  if (!command_return) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "wpa_supplicant did not answer, could not enable new network!";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose) << reply;
  network_id_select = network_id;
}
