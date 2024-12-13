#include "sht25_handler.hpp"

#include "wolf/low_pass_filter.hpp"
#include "wolf/sensor_id_sht25.hpp"

#include <boost/optional/optional_io.hpp>

using namespace logging;
using namespace wolf;

sht25_handler::sht25_handler(
    const low_pass_filter_creator &filter_creator_temperature,
    const low_pass_filter_creator &filter_creator_humidity)
    : m_logger("sht25_handler"),
      m_filter_temperature{filter_creator_temperature.create()},
      m_filter_humidity{filter_creator_humidity.create()} {}

sht25_handler::~sht25_handler() {}

void sht25_handler::handle_read(const sensor_sht25_channel channel,
                                const sht25_handler::read_buffer &data) {
  const types::data data_casted(data.cbegin(), data.cend());
  handle_read(channel, data_casted);
}

void sht25_handler::handle_read(const sensor_sht25_channel channel,
                                const types::data &data) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_read, id:" << channel << " data:" << data;
  if (data.size() != 3) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_read with invalid data size, id:" << channel
        << " size:" << data.size();
    return;
  }
  const sht25_parser::read_buffer data_casted = {{data[0], data[1], data[2]}};
  const float value = m_parser.parse(channel, data_casted);
  auto &low_pass_filter = get_smoother(channel);
  const auto before = low_pass_filter.get_filtered_value();
  bool emit_signals = low_pass_filter.handle_value(value);
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_read smoother, result:" << std::boolalpha << emit_signals
      << " value:" << value << " before:" << before
      << " get():" << low_pass_filter.get_filtered_value();
  if (!emit_signals) return;
  const auto now = std::chrono::system_clock::now();
  const auto id = sensor_id_sht25::create(channel);
  signal_value({id, value, now});
}

void sht25_handler::resend_signals() {
  const auto last_temperature = m_filter_temperature.get_filtered_value();
  const auto last_humidity = m_filter_humidity.get_filtered_value();
  const auto now = std::chrono::system_clock::now();
  if (last_temperature.has_value()) {
    static const auto id =
        sensor_id_sht25::create(sensor_sht25_channel::temperature);
    signal_value({id, last_temperature.get(), now});
  }
  if (last_humidity.has_value()) {
    static const auto id =
        sensor_id_sht25::create(sensor_sht25_channel::humidity);
    signal_value({id, last_humidity.get(), now});
  }
}

low_pass_filter &sht25_handler::get_smoother(
    const sensor_sht25_channel channel) {
  if (channel == sensor_sht25_channel::temperature) return m_filter_temperature;
  return m_filter_humidity;
}
