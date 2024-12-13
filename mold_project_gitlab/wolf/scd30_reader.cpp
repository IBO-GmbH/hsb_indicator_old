#include "scd30_reader.hpp"
#include "i2c/open_close.hpp"
#include "std_array_printer.hpp"
#include "wolf/sensirion/crc.hpp"

using namespace wolf;
using namespace logging;

scd30_reader::scd30_reader()
    : m_logger{"scd30_reader"}, m_reader_writer(m_logger, m_file) {}

bool scd30_reader::open(const std::string &device, const int slave_id) {
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

  return check_open();
}

void scd30_reader::close() { i2c::open_close::close(m_file); }

bool scd30_reader::trigger_continuous_measurement(int interval) {
  if (interval < 2) {
    LOGGING_LOG(m_logger, severity::warning)
        << "interval below 2 => defaulting to 2";
    interval = 2;
  }

  if (interval > 1800) {
    LOGGING_LOG(m_logger, severity::warning)
        << "interval above 1800 => defaulting to 1800";
    interval = 1800;
  }

  if (!send_command_with_argument(m_continuous_measurement_interval,
                                  static_cast<std::uint16_t>(interval)))
    return false;
  return send_command_with_argument(m_trigger_continuous_measurement, 0);
}

bool scd30_reader::set_asc_active(const bool active) {
  return send_command_with_argument(m_activate_asc, active ? 1 : 0);
}

std::optional<scd30_reader::read_buffer> scd30_reader::get_data_ready_status() {
  if (!send_command(m_get_data_ready_status)) return {};
  read_buffer read_buffer_;
  if (!m_reader_writer.read_into_buffer(read_buffer_)) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not read get_data_ready_status";
    return {};
  }

  LOGGING_LOG(m_logger, severity::verbose)
      << std_array_printer::to_string(read_buffer_);
  return read_buffer_;
}

std::optional<scd30_reader::read_data_buffer> scd30_reader::get_data() {
  if (!send_command(m_get_data)) return {};
  read_data_buffer read_buffer_;

  if (!m_reader_writer.read_into_buffer(read_buffer_)) {
    LOGGING_LOG(m_logger, severity::warning) << "could not read get_data";
    return {};
  }

  LOGGING_LOG(m_logger, severity::verbose)
      << std_array_printer::to_string(read_buffer_);
  return read_buffer_;
}

bool scd30_reader::check_open() {
  char check_byte = 0;
  return m_reader_writer.write_byte(check_byte);
}

bool scd30_reader::send_command(uint16_t command) {
  std::array<std::uint8_t, 2> to_send;
  to_send[0] = (command >> 8) & 0xFF;
  to_send[1] = (command >> 0) & 0xFF;
  return m_reader_writer.write_buffer(to_send);
}

bool scd30_reader::send_command_with_argument(const uint16_t command,
                                              const uint16_t argument) {
  std::array<std::uint8_t, 5> to_send;
  to_send[0] = (command >> 8) & 0xFF;
  to_send[1] = (command >> 0) & 0xFF;
  to_send[2] = (argument >> 8) & 0xFF;
  to_send[3] = (argument >> 0) & 0xFF;
  to_send[4] = sensirion::crc::from_uint16(argument);
  LOGGING_LOG(m_logger, severity::verbose)
      << "send_command_with_argument:" << std_array_printer::to_string(to_send);
  return m_reader_writer.write_buffer(to_send);
}
