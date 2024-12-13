#ifndef MOLD_GUI_WIFIHANDLER_HPP
#define MOLD_GUI_WIFIHANDLER_HPP

#include "wifi/active_access_point_getter.hpp"
#include "wifi/device_state_watcher.hpp"
#include "wifi/rescanner.hpp"
#include "wifi/scanner.hpp"

namespace mold {

class WifiHandler {
 public:
  static bool initialise(
      std::unique_ptr<wifi::scanner> &scanner,
      std::unique_ptr<wifi::rescanner> &rescanner,
      std::unique_ptr<wifi::device_state_watcher> &watcher,
      std::unique_ptr<wifi::active_access_point_getter> &active_getter);
};
}  // namespace mold

#endif  // MOLD_GUI_WIFIHANDLER_HPP
