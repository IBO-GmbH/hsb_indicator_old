#include "sgpc3_reader.hpp"
#include <thread>
#include "wolf/i2c/open_close.hpp"

using namespace wolf;
using namespace logging;

static constexpr std::uint16_t m_init_air_qualitiy_0s = 0x2089;
static constexpr std::uint16_t m_init_air_qualitiy_64s = 0x2003;
static constexpr std::uint16_t m_init_air_qualitiy_184s = 0x206a;
static constexpr int m_init_air_qualitiy_wait_ms = 10;
static constexpr std::uint16_t m_measure_air_qualitiy = 0x2008;
static constexpr int m_measure_air_qualitiy_wait_ms = 50;

sgpc3_reader::sgpc3_reader(boost::asio::io_service &service)
    : m_logger{"sgpc3_reader"},
      m_timer{service},
      m_reader_writer{m_logger, m_file} {}

bool wolf::sgpc3_reader::open(const std::string &device, const int slave_id) {
  if (!open_file(device, slave_id)) return false;
  if (!send_command(m_init_air_qualitiy_0s)) return false;
  const int wait_for = m_init_air_qualitiy_wait_ms;
  std::this_thread::sleep_for(std::chrono::milliseconds(wait_for));
  return true;
}

void sgpc3_reader::close() {
  LOGGING_LOG(m_logger, severity::normal) << "closing";
  boost::system::error_code ignore_error;  // may occur if already closed
  m_timer.cancel(ignore_error);
  i2c::open_close::close(m_file);
}

bool sgpc3_reader::read_data_async() {
  const bool result = send_command(m_measure_air_qualitiy);
  m_timer.expires_from_now(
      boost::posix_time::milliseconds(m_measure_air_qualitiy_wait_ms));
  m_timer.async_wait([this](const boost::system::error_code &error) {
    handle_read_data(error);
  });
  return result;
}

bool sgpc3_reader::open_file(const std::string &device, const int slave_id) {
  LOGGING_LOG(m_logger, severity::normal)
      << "opening device:" << device << " using slave_id:0x" << std::hex
      << slave_id;
  if (auto file = i2c::open_close::open(device, slave_id); file.has_value()) {
    m_file = *file;
  } else {
    LOGGING_LOG(m_logger, severity::normal)
        << "opening device:" << device << " failed";
    return false;
  }
  return true;
}

bool sgpc3_reader::check_open(std::array<std::uint8_t, 2> &to_send) {
  const bool result = m_reader_writer.write_buffer(to_send);
  if (!result)
    LOGGING_LOG(m_logger, severity::warning) << "device not able to write";
  return result;
}

bool sgpc3_reader::send_command(uint16_t command) {
  std::array<std::uint8_t, 2> to_send;
  to_send[0] = (command >> 8) & 0xFF;
  to_send[1] = (command >> 0) & 0xFF;
  return check_open(to_send);
}

void sgpc3_reader::handle_read_data(const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "handle_read_data, error:" << error.message();
    return;
  }

  read_buffer buffer;
  m_reader_writer.read_into_buffer(buffer);
  signal_read_data(buffer);
}
