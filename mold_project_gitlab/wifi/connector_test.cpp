#include "wifi/connector.hpp"
#include <QCoreApplication>
#include <iostream>
#include "log/log.hpp"
#include "log/logger.hpp"

int main(int argc, char* argv[]) {
  logging::initialize();
  logging::logger main_logger{"main_logger"};
  LOGGING_LOG(main_logger, logging::severity::verbose) << "starting";

  QCoreApplication app(argc, argv);

  if (argc != 3) {
    std::cerr << "Usage: wifi_connector_test <ssid> <password>" << std::endl;
    return 1;
  }

  std::string ssid = argv[1];
  std::string password = argv[2];
  auto on_success = [&] {
    LOGGING_LOG(main_logger, logging::severity::normal)
        << "successfully connected";
    app.exit();
  };
  auto on_failure = [&] {
    LOGGING_LOG(main_logger, logging::severity::warning)
        << "failed to connect";
    app.exit(2);
  };
  const wifi::connector connector(ssid, password, on_success, on_failure);
  return app.exec();
}
