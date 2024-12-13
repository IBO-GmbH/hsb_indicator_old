#include "stats_reader.hpp"

using namespace logging;

stats_reader::stats_reader(boost::asio::io_service& service_)
    : m_logger{"stats_reader"},
      service(service_),
      serial(service),
      timer(service),
      read_write(serial, "stats_reader"),
      rssi(read_write) {
  rssi.signal_read.connect([this] { handle_read_rssi(); });
}

void stats_reader::open(
    boost::interprocess::managed_shared_memory& shared_memory,
    const std::string& path) {
  LOGGING_LOG(m_logger, severity::normal)
      << "stats_reader, opening path:" << path;
  rssi.construct_memory(shared_memory);
  serial.open(path);
}

void stats_reader::close() {
  boost::system::error_code error;
  // let's irgnore any errors
  timer.cancel(error);
  serial.close(error);
}

void stats_reader::run() { get_next(); }

void stats_reader::get_next() {
  timer.expires_from_now(std::chrono::seconds(10));
  timer.async_wait([this](const boost::system::error_code& error) {
    handle_timeout(error);
  });
}

void stats_reader::handle_timeout(const boost::system::error_code& error) {
  if (error == boost::asio::error::operation_aborted) return;
  if (error) throw boost::system::system_error(error);

  rssi.get_next();
}

void stats_reader::handle_read_rssi() { get_next(); }
