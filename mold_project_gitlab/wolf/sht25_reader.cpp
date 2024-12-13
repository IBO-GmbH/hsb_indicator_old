#include "sht25_reader.hpp"
#include <linux/i2c-dev.h>
#include <bitset>
#include <thread>
#include "i2c/open_close.hpp"
#include "wolf/i2c/open_close.hpp"

using namespace logging;
using namespace wolf;

static constexpr char m_soft_reset{static_cast<char>(0b11111110)};
static constexpr char m_user_register{static_cast<char>(0b11100111)};
static constexpr char m_temperature_measurement{static_cast<char>(0b11110011)};
static constexpr int m_wait_for_temperature_measurement{
    85};  // milliseconds (datasheet Table7)
static constexpr char m_relative_humidity_measurement{
    static_cast<char>(0b11110101)};
static constexpr int m_wait_for_relative_humidity_measurement{
    29};  // milliseconds (datasheet Table7)

sht25_reader::sht25_reader(boost::asio::io_service &service)
    : m_logger{"sht25_reader"},
      m_service(service),
      m_timer(m_service),
      m_read_interval{},
      reader_writer(m_logger, m_file) {}

bool sht25_reader::open(const std::string &device, const int slave_id) {
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

  if (!do_soft_reset_sync()) return false;
  return print_user_register();
}

void sht25_reader::close() {
  boost::system::error_code ignore_error;
  m_timer.cancel(ignore_error);
  i2c::open_close::close(m_file);
}

bool sht25_reader::read_async(int read_interval) {
  if (read_interval < 4) {
    LOGGING_LOG(m_logger, severity::warning)
        << "read_interval below 4, defaulting to 4";
    read_interval = 4;
  }
  m_read_interval = read_interval;
  return read_temperature();
}

bool sht25_reader::do_soft_reset_sync() {
  LOGGING_LOG(m_logger, severity::normal) << "doing a soft reset";
  const auto result = check_open(m_soft_reset);
  std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(15));
  return result;
}

bool sht25_reader::print_user_register() {
  // lets read the user register
  if (!reader_writer.write_byte(m_user_register)) {
    LOGGING_LOG(m_logger, severity::warning)
        << "print_user_register() failed because writing failed";
    return false;
  }

  char user_regiser;
  LOGGING_LOG(m_logger, severity::verbose) << "reading user_register";
  if (!reader_writer.read_byte(user_regiser)) {
    LOGGING_LOG(m_logger, severity::warning)
        << "print_user_register() failed because reading failed";
    return false;
  }

  std::bitset<8> user_regiser_print(user_regiser);
  LOGGING_LOG(m_logger, severity::normal)
      << "user_register, resolution(7,0):" << std::dec << user_regiser_print[7]
      << " " << user_regiser_print[0] << " status(6):" << user_regiser_print[6]
      << " reserved(3,4,5):" << user_regiser_print[3] << " "
      << user_regiser_print[4] << " " << user_regiser_print[5]
      << " heater(2):" << user_regiser_print[2]
      << " OTP(1):" << user_regiser_print[1];
  return true;
}

bool sht25_reader::check_open(const char check_byte) {
  if (!reader_writer.write_byte(check_byte)) {
    LOGGING_LOG(m_logger, severity::warning)
        << "device not opened succsessfully";
    return false;
  }
  return true;
}

bool sht25_reader::read_temperature() {
  if (!reader_writer.write_byte(m_temperature_measurement)) return false;
  m_timer.expires_from_now(
      boost::posix_time::milliseconds(m_wait_for_temperature_measurement));
  m_timer.async_wait([this](const boost::system::error_code &error) {
    handle_read_temperature(error);
  });
  return true;
}

void sht25_reader::handle_read_temperature(
    const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "sht25_reader::handle_read_temperature, error:" << error.message();
    return;
  }

  if (!reader_writer.read_into_buffer(m_read_buffer_temperature)) {
    return;
  }
  signal_read_temperature(m_read_buffer_temperature);
  read_humidity();
}

bool sht25_reader::read_humidity() {
  if (!reader_writer.write_byte(m_relative_humidity_measurement)) return false;

  m_timer.expires_from_now(boost::posix_time::milliseconds(
      m_wait_for_relative_humidity_measurement));
  m_timer.async_wait([this](const boost::system::error_code &error) {
    handle_read_humidity(error);
  });
  return true;
}

void sht25_reader::handle_read_humidity(
    const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "sht25_reader::handle_read_humidity, error:" << error.message();
    return;
  }

  if (!reader_writer.read_into_buffer(m_read_buffer_humidity)) {
    LOGGING_LOG(m_logger, severity::warning) << "failed to read humidity";
    return;
  }

  signal_read_humidity(m_read_buffer_humidity);
  signal_read(m_read_buffer_temperature, m_read_buffer_humidity);

  m_timer.expires_from_now(boost::posix_time::seconds(m_read_interval));
  m_timer.async_wait([this](const boost::system::error_code &error) {
    if (error) {
      LOGGING_LOG(m_logger, severity::warning)
          << "timer error:" << error.message();
      return;
    }
    read_temperature();
  });
}
