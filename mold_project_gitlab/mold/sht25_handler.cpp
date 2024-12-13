#include "sht25_handler.hpp"
#include "wolf/low_pass_filter_creator.hpp"
#include "wolf/program_options_parser.hpp"
#include "wolf/sensor_id_sht25.hpp"

using namespace mold;

sht25_handler::sht25_handler(const wolf::config& program_options,
                             wolf::sht25_reader& reader)
    : m_logger{"sht25_handler"},
      m_reader(reader),
      filter_temperature(
          {program_options.sht25_.temperature_filter_constant,
           program_options.sht25_.temperature_filter_threshold,
           program_options.sht25_.temperature_filter_threshold_with_time,
           std::chrono::seconds(
               program_options.sht25_.filter_threshold_timeout)}),
      filter_humidity(
          {program_options.sht25_.humidity_filter_constant,
           program_options.sht25_.humidity_filter_threshold,
           program_options.sht25_.humidity_filter_threshold_with_time,
           std::chrono::seconds(
               program_options.sht25_.filter_threshold_timeout)}),
      handler(filter_temperature, filter_humidity),
      read_interval(program_options.sht25_.read_interval) {
  reader.signal_read_temperature.connect(
      [this](const wolf::sht25_reader::read_buffer& data) {
        handler.handle_read(wolf::sensor_sht25_channel::temperature, data);
      });
  reader.signal_read_humidity.connect(
      [this](const wolf::sht25_reader::read_buffer& data) {
        handler.handle_read(wolf::sensor_sht25_channel::humidity, data);
      });
}

bool sht25_handler::start_reading() {
  if (!m_reader.read_async(read_interval)) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "could not start reading";
    return false;
  }
  return true;
}

void sht25_handler::close() { m_reader.close(); }

void sht25_handler::resend_signals() { handler.resend_signals(); }
