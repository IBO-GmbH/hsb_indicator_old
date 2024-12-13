#include "wifi/scanner.hpp"
#include <QCoreApplication>
#include <boost/optional/optional_io.hpp>
#include "log/log.hpp"
#include "log/logger.hpp"
#include "wifi/access_point_getter.hpp"
#include "wifi/active_access_point_getter.hpp"
#include "wifi/device_finder.hpp"
#include "wifi/device_state_watcher.hpp"

void print_current_ssid(logging::logger logger,
                        wifi::active_access_point_getter& getter);

int main(int argc, char* argv[]) {
  logging::initialize();
  logging::logger main_logger{"main_logger"};
  LOGGING_LOG(main_logger, logging::severity::verbose) << "starting";

  QCoreApplication app(argc, argv);
  wifi::device_finder finder;
  boost::optional<std::string> wifi_device = finder.get_first_wifi_device();
  if (!wifi_device) {
    LOGGING_LOG(main_logger, logging::severity::error)
        << "could not find a wifi device. Exiting!";
    return 1;
  }
  wifi::device_state_watcher state_watcher(wifi_device.get());
  wifi::active_access_point_getter active_access_point_getter(
      wifi_device.get());
  print_current_ssid(main_logger, active_access_point_getter);
  state_watcher.signal_state_changed.connect(
      [&](const wifi::device_state state) {
        LOGGING_LOG(main_logger, logging::severity::normal)
            << "state changed to:" << state;
        print_current_ssid(main_logger, active_access_point_getter);
      });
  wifi::scanner scanner(wifi_device.get());
  for (const auto& access_point : scanner.get_access_points())
    LOGGING_LOG(main_logger, logging::severity::normal)
        << "access_point:" << access_point;
  scanner.signal_added.connect([&](const wifi::access_point& added) {
    LOGGING_LOG(main_logger, logging::severity::normal)
        << "wifi added:" << added;
  });
  scanner.signal_removed.connect([&](const std::string& removed) {
    LOGGING_LOG(main_logger, logging::severity::normal)
        << "wifi removed:" << removed;
  });

  return app.exec();
}

void print_current_ssid(logging::logger logger,
                        wifi::active_access_point_getter& getter) {
  boost::optional<std::string> active_access_point = getter.get();
  if (!active_access_point) {
    LOGGING_LOG(logger, logging::severity::normal)
        << "current access_point:" << active_access_point;
    return;
  }
  wifi::access_point_getter access_point_getter(active_access_point.get());
  const wifi::access_point access_point = access_point_getter.get();
  LOGGING_LOG(logger, logging::severity::normal)
      << "current access_point:" << access_point;
}
