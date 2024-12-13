#include "gsm_information_reader.hpp"

using namespace logging;

const std::string mold::gsm_information_reader::shared_memory_name =
    "GsmInformation";

mold::gsm_information_reader::gsm_information_reader(
    boost::asio::io_service &service, const mold::config &config_)
    : m_logger("gsm_information_reader"), m_config(config_), m_timer(service) {
  read_from_shared_memory();
}

mold::gsm_information_reader::~gsm_information_reader() { cancel_timer(); }

mold::gsm_information_reader::optional_memory
mold::gsm_information_reader::open_shared_memory() const {
  try {
    return boost::interprocess::managed_shared_memory(
        boost::interprocess::open_only, shared_memory_name.data());
  } catch (std::exception &error) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "could not open shared memory, " << error.what();
    return optional_memory();
  }
}

void mold::gsm_information_reader::read_from_shared_memory() {
  LOGGING_LOG(m_logger, severity::verbose) << "read from shared memory";
  auto shared_memory = open_shared_memory();
  if (shared_memory.has_value()) {
    read_iccid(shared_memory.value());
    read_rssi(shared_memory.value());
    read_provider(shared_memory.value());
  }
  start_timer();
}

void mold::gsm_information_reader::read_iccid(
    mold::gsm_information_reader::memory &shared_memory) const {
  LOGGING_LOG(m_logger, severity::verbose) << "read iccid from shared memory";
  auto iccid = shared_memory.find<ipc_string>("iccid");
  if (!iccid.first) {
    LOGGING_LOG(m_logger, severity::warning)
        << "iccid not found in shared memory";
    signal_iccid(optional_string());
    return;
  }
  const std::string iccid_string(iccid.first->data());
  if (iccid_string.empty()) {
    signal_iccid(optional_string());
    return;
  }
  signal_iccid(iccid_string);
}

void mold::gsm_information_reader::read_rssi(
    mold::gsm_information_reader::memory &shared_memory) const {
  LOGGING_LOG(m_logger, severity::verbose) << "read rssi from shared memory";
  auto rssi = shared_memory.find<std::int32_t>("rssi");
  if (!rssi.first) {
    LOGGING_LOG(m_logger, severity::warning)
        << "rssi not found in shared memory";
    signal_rssi(optional_int());
    return;
  }
  signal_rssi(*rssi.first);
}

void mold::gsm_information_reader::read_provider(
    mold::gsm_information_reader::memory &shared_memory) const {
  LOGGING_LOG(m_logger, severity::verbose)
      << "read provider from shared memory";
  auto provider = shared_memory.find<ipc_string>("provider");
  if (!provider.first) {
    LOGGING_LOG(m_logger, severity::warning)
        << "provider not found in shared memory";
    signal_provider(optional_string());
    return;
  }
  const std::string provider_string(provider.first->data());
  if (provider_string.empty()) {
    signal_provider(optional_string());
    return;
  }
  signal_provider(provider_string);
}

void mold::gsm_information_reader::start_timer() {
  if (m_time_interval.count() == 0) return;
  LOGGING_LOG(m_logger, severity::verbose)
      << "starting timer, expires in: "
      << std::chrono::duration_cast<std::chrono::seconds>(m_time_interval)
             .count()
      << " seconds";

  m_timer.expires_from_now(m_time_interval);
  m_timer.async_wait(
      [this](const boost::system::error_code &error) { handle_wait(error); });
}

void mold::gsm_information_reader::cancel_timer() {
  boost::system::error_code error;
  m_timer.cancel(error);
  if (error)
    LOGGING_LOG(m_logger, severity::warning)
        << "could not cancel timer, " << error.message();
}

void mold::gsm_information_reader::handle_wait(
    const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "gsm_information_reader::handle_wait, error: " << error.message();
    return;
  }
  read_from_shared_memory();
}
