#include <wiringPi.h>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include "log/log.hpp"
#include "log/logger.hpp"
#include "wolf/tone_writer.hpp"

using namespace logging;

int main(int /*argc*/, char* /*argv*/[]) {
  initialize();
  logger main_logger("main");

  LOGGING_LOG(main_logger, severity::normal) << "starting up";

  if (wiringPiSetup() != 0) {
    LOGGING_LOG(main_logger, severity::error) << "failed to set up wiring pi";
    return 1;
  }

  boost::asio::io_service service;
  boost::asio::deadline_timer timer(service);
  bool end(false);

  wolf::tone_writer writer{17};
  if (!writer.open()) throw std::runtime_error("could not open tone_writer");
  writer.set_enabled(true);

  boost::asio::signal_set signals_to_listen_to(service, SIGINT, SIGTERM);
  signals_to_listen_to.async_wait(
      [&](const boost::system::error_code& error, int /*signal_number*/) {
        if (error) {
          LOGGING_LOG(main_logger, severity::error)
              << "signal_handler called with error:" << error.message();
          return;
        }
        LOGGING_LOG(main_logger, severity::normal) << "shutting down";
        timer.cancel();
        end = true;
      });
  service.run();
  writer.set_enabled(false);
  return 0;
}
