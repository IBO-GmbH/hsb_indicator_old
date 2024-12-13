#include "WifiHandler.hpp"

#include "wifi/device_finder.hpp"

bool mold::WifiHandler::initialise(
    std::unique_ptr<wifi::scanner> &scanner,
    std::unique_ptr<wifi::rescanner> &rescanner,
    std::unique_ptr<wifi::device_state_watcher> &watcher,
    std::unique_ptr<wifi::active_access_point_getter> &active_getter) {
  wifi::device_finder finder;
  boost::optional<std::string> wifi_device = finder.get_first_wifi_device();
  if (!wifi_device) {
    scanner.reset(nullptr);
    rescanner.reset(nullptr);
    watcher.reset(nullptr);
    active_getter.reset(nullptr);
    return false;
  }
  scanner.reset(new wifi::scanner(wifi_device.get()));
  rescanner.reset(new wifi::rescanner());
  watcher.reset(new wifi::device_state_watcher(wifi_device.get()));
  active_getter.reset(new wifi::active_access_point_getter(wifi_device.get()));
  return true;
}
