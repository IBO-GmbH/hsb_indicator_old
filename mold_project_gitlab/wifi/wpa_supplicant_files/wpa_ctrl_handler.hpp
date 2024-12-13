#ifndef WIFI_WPA_CTRL_HANDLER_HPP
#define WIFI_WPA_CTRL_HANDLER_HPP

#include <optional>
#include "log/logger.hpp"
#include "wpa_ctrl.hpp"

namespace wifi {

class wpa_ctrl_handler {
 public:
  wpa_ctrl_handler();
  bool check_if_open(wpa_ctrl* ctrl) const;
  void list_all_available_networks(wpa_ctrl* ctrl) const;
  void list_all_connected_networks(wpa_ctrl* ctrl) const;
  void connect_to_network(
      wpa_ctrl* ctrl, const std::string& name,
      const std::function<std::string()>& password_callback) const;
  void display_current_network(wpa_ctrl* ctrl) const;
  void remove_network(wpa_ctrl* ctrl, const std::string& name) const;
  void save_configuartion(wpa_ctrl* ctrl) const;

 private:
  bool handle_command(wpa_ctrl* ctrl, const std::string& command,
                      std::string& reply) const;
  using optional_string = std::optional<std::string>;
  optional_string get_all_connected_networks(wpa_ctrl* ctrl) const;
  optional_string get_single_network_from_list(
      wpa_ctrl* ctrl, const std::string& to_search) const;
  optional_string get_id_from_network_line(const std::string& network) const;
  static bool is_id(const std::string& to_check);
  void add_new_network(
      wpa_ctrl* ctrl, const std::string& name,
      optional_string& network_id_select,
      const std::function<std::string()>& password_callback) const;

 private:
  mutable logging::logger m_logger;
};
}  // namespace wifi

#endif  // WIFI_WPA_CTRL_HANDLER_HPP
