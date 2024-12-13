#include "rescanner.hpp"
#include <boost/process.hpp>
#include <chrono>
#include <thread>

using namespace wifi;
using namespace logging;

rescanner::rescanner(QObject* parent)
    : QObject(parent), m_logger{"wifi::rescanner"} {}

void rescanner::rescan() {
  if (m_fut.valid()) {
    LOGGING_LOG(m_logger, severity::verbose) << "fut is valid";
    const auto status = m_fut.wait_for(std::chrono::milliseconds(0));
    LOGGING_LOG(m_logger, severity::verbose)
        << "fut status:" << static_cast<int>(status);
    if (status != std::future_status::ready) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "previous rescan still running thread id:"
          << std::this_thread::get_id();
      return;
    }
  } else {
    LOGGING_LOG(m_logger, severity::verbose) << "fut is invalid";
  }

  m_fut = std::async([this] {
    LOGGING_LOG(m_logger, severity::verbose)
        << "executing nmcli device wifi rescan";
    int exit_code = -1;
    try {
      exit_code =
          boost::process::system("/usr/bin/nmcli", "device", "wifi", "rescan");
    } catch (const std::runtime_error& re) {
      LOGGING_LOG(m_logger, severity::warning)
          << "runtime error: " << re.what();
    } catch (const std::exception& ex) {
      LOGGING_LOG(m_logger, severity::warning)
          << "error occurred: " << ex.what();
    } catch (...) {
      LOGGING_LOG(m_logger, severity::warning) << "unknown failure occurred";
    }

    if (exit_code != 0) {
      LOGGING_LOG(m_logger, severity::warning)
          << "could not execute nmcli device wifi rescan, exit code = "
          << std::to_string(exit_code);
    } else {
      LOGGING_LOG(m_logger, severity::verbose)
          << "successfully executed nmcli device wifi rescan";
    }
  });
}
