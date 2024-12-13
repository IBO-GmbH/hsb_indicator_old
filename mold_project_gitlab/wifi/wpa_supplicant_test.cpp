#include <iostream>
#include "log/log.hpp"
#include "wpa_supplicant_files/wpa_ctrl_handler.hpp"

enum struct options {
  invalid,
  list_all_available,
  list_all_connected,
  connect_to,
  get_current,
  remove,
  save_config,
  end,
};

using namespace wifi;

int main(int /*argc*/, char* /*argv*/[]) {
  std::cout << "Start Test Program" << std::endl;
  logging::initialize();
  auto ctrl = wpa_ctrl_open("/var/run/wpa_supplicant/wlan0");
  if (ctrl == nullptr) {
    std::cerr << "could not get control interface of wpa_supplicant!"
              << std::endl;
    return -1;
  }
  wpa_ctrl_handler ctrl_handler;
  if (!ctrl_handler.check_if_open(ctrl)) return -2;
  while (1) {
    std::cout << std::endl
              << "1 - List all available Wifi" << std::endl
              << "2 - List all connected Wifi" << std::endl
              << "3 - Connect to Wifi (Name or ID)" << std::endl
              << "4 - Get connected Wifi" << std::endl
              << "5 - Remove Wifi (Name or ID)" << std::endl
              << "6 - Save Configuration" << std::endl
              << "7 - End" << std::endl;
    options option = options::invalid;
    int input;
    std::cin >> input;
    option = static_cast<options>(input);
    std::string name;
    switch (option) {
      case options::list_all_available:
        ctrl_handler.list_all_available_networks(ctrl);
        break;
      case options::list_all_connected:
        ctrl_handler.list_all_connected_networks(ctrl);
        break;
      case options::connect_to:
        std::cout << "connect to which network?" << std::endl;
        std::cin >> name;
        ctrl_handler.connect_to_network(ctrl, name, []() {
          std::cout << "password of network?" << std::endl;
          std::string password;
          std::cin >> password;
          return password;
        });
        break;
      case options::get_current:
        ctrl_handler.display_current_network(ctrl);
        break;
      case options::remove:
        std::cout << "remove which network?" << std::endl;
        std::cin >> name;
        ctrl_handler.remove_network(ctrl, name);
        break;
      case options::save_config:
        ctrl_handler.save_configuartion(ctrl);
        break;
      case options::end:
        std::cout << "End Test Program" << std::endl;
        wpa_ctrl_close(ctrl);
        return 0;
      default:
        std::cout << "invalid option!" << std::endl;
        break;
    }
    continue;
  }
}
