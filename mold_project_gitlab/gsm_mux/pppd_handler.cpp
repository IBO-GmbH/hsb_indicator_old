#include "pppd_handler.hpp"
#include <boost/process.hpp>

using namespace logging;

pppd_handler::pppd_handler(boost::asio::io_service& service_,
                           const std::string& isp_name,
                           pppd_output_checker& output)
    : m_logger{"pppd_handler"},
      service(service_),
      isp_name(isp_name),
      output(output) {}

void pppd_handler::run() {
  LOGGING_LOG(m_logger, severity::normal) << "run";
  shall_retry = true;
  retry();
}

void pppd_handler::stop() {
  LOGGING_LOG(m_logger, severity::normal) << "stop()";
  shall_retry = false;
  {
    boost::system::error_code error;
    retry_timer.cancel(error);
    if (error)
      LOGGING_LOG(m_logger, severity::error)
          << "an error occured, when trying to stop the retry timer. error:"
          << error.message();
  }
  if (child) {
    std::error_code error;
    child->terminate(error);
    if (error)
      LOGGING_LOG(m_logger, severity::error)
          << "an error occured, when trying to stop pppd. error:"
          << error.message();
    child.reset(nullptr);
  }
}

void pppd_handler::retry() {
  namespace bp = boost::process;

  LOGGING_LOG(m_logger, severity::normal) << "retry() starting pppd";

  wait_for_child.async_wait([this](const boost::system::error_code& error,
                                   int /*signal*/) { handle_exit(error); });
  output.reset();
  auto& pipe = output.get_pipe();
  child = std::make_unique<boost::process::child>(
      "/usr/bin/pon", isp_name,
      (boost::process::std_out & boost::process::std_err) > pipe);
  output.start();
}

void pppd_handler::handle_exit(const boost::system::error_code& error) {
  LOGGING_LOG(m_logger, severity::warning) << "handle_exit";
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_exit, an error occured:" << error.message();
    return;
  }
  int status;
  ::wait(&status);
  LOGGING_LOG(m_logger, severity::normal)
      << "pppd exited with status:" << status;
  if (shall_retry) {
    // maybe it was just an pppd issue
    retry_timer.expires_from_now(std::chrono::seconds(4));
    retry_timer.async_wait([this](const boost::system::error_code& error) {
      handle_retry_timeout(error);
    });
  }
}

void pppd_handler::handle_retry_timeout(
    const boost::system::error_code& error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::normal)
        << "handle_retry_timeout, error:" << error;
    return;
  }
  run();
}
