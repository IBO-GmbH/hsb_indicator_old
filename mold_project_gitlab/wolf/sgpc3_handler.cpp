#include "sgpc3_handler.hpp"

#include "wolf/sensor_id_sgpc3.hpp"

#include <boost/optional/optional_io.hpp>

using namespace logging;

wolf::sgpc3_handler::sgpc3_handler(boost::asio::io_service& service,
                                   sgpc3_reader& reader,
                                   const config& program_options)
    : m_logger("sgpc3_handler"),
      m_timer_read(service),
      m_reader(reader),
      m_filter_voc({program_options.sgpc3_.voc_filter_constant,
                    program_options.sgpc3_.voc_filter_threshold,
                    program_options.sgpc3_.voc_filter_threshold_with_time,
                    seconds(program_options.sgpc3_.filter_threshold_timeout)}) {
}

void wolf::sgpc3_handler::start_reading() {
  LOGGING_LOG(m_logger, severity::verbose) << "start reading from sgpc3";
  m_reader.signal_read_data.connect(
      [this,
       id = wolf::sensor_id_sgpc3::create(wolf::sensor_sgpc3_channel::tvoc)](
          const wolf::sgpc3_reader::read_buffer& buffer) {
        handle_read(buffer, id);
      });
  m_next_read = std::chrono::steady_clock::now();
  read();
}

void wolf::sgpc3_handler::close() {
  boost::system::error_code ignore_error;
  m_timer_read.cancel(ignore_error);
}

void wolf::sgpc3_handler::resend_signal() {
  const auto last_value = m_filter_voc.get_filtered_value();
  if (!last_value.has_value()) return;
  const auto now = std::chrono::system_clock::now();
  const auto id =
      wolf::sensor_id_sgpc3::create(wolf::sensor_sgpc3_channel::tvoc);
  signal_value({id, last_value.get(), now});
}

void wolf::sgpc3_handler::read() {
  m_next_read += time_to_measure;
  m_timer_read.expires_at(m_next_read);
  m_timer_read.async_wait([&](const boost::system::error_code& error) {
    if (error) return;
    if (!m_reader.read_data_async()) {
      LOGGING_LOG(m_logger, severity::warning)
          << "an error occured when trying to read from sensor";
      read();
    }
  });
}

void wolf::sgpc3_handler::handle_read(
    const wolf::sgpc3_reader::read_buffer& buffer, const wolf::sensor_id& id) {
  const auto result = m_parser.handle_data(buffer);
  if (result) {
    float value = result.get();
    if (m_filter_handler.filter_value(sensor_type::sgpc3, m_filter_voc,
                                      value)) {
      LOGGING_LOG(m_logger, severity::normal) << "tvoc: " << value;
      const auto now = std::chrono::system_clock::now();
      signal_value({id, value, now});
    }
  }
  read();
}
