#include "esp3_sender.hpp"

#include "esp3_crc.hpp"
#include "serial_interface.hpp"

#include <sstream>

using namespace logging;
using namespace wolf;

esp3_sender::esp3_sender(serial_interface &serial)
    : m_serial(serial), m_logger{"esp3_sender"} {}

void esp3_sender::send_rps(const uint8_t &send_, const uint8_t &status) {
  send_rps(send_, 0x00, status);
}

void esp3_sender::send_rps(const uint8_t &send_,
                           const types::id_esp3 &sender_id,
                           const uint8_t &status) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "send_rps, sender_id:" << std::hex << sender_id
      << " send:" << static_cast<int>(send_);
  send_telegram({send_}, RORG_RPS, sender_id, status);
}

void esp3_sender::send_telegram(const types::data &embedded_data,
                                const RORG_TYPE &org,
                                const types::id_esp3 &sender_id,
                                const uint8_t &status) {
  std::vector<std::uint8_t> data;
  {
    data.push_back(org);
    std::copy(embedded_data.cbegin(), embedded_data.cend(),
              std::back_inserter(data));
    data.push_back((sender_id >> 24) & 0xff);
    data.push_back((sender_id >> 16) & 0xff);
    data.push_back((sender_id >> 8) & 0xff);
    data.push_back((sender_id >> 0) & 0xff);
    // status
    data.push_back(status);
    // subtelegram count
    data.push_back(0x03);
    const std::uint32_t destination = BROADCAST_ID;
    data.push_back((destination >> 24) & 0xff);
    data.push_back((destination >> 16) & 0xff);
    data.push_back((destination >> 8) & 0xff);
    data.push_back((destination >> 0) & 0xff);
    // no idea!
    data.push_back(0xff);
    data.push_back(0x00);
  }

  std::stringstream bytes;
  for (const auto &byte : data)
    bytes << " " << std::hex << static_cast<int>(byte);

  bytes.flush();
  LOGGING_LOG(m_logger, severity::verbose) << "sending bytes: " << bytes.str();
  send(data, PACKET_RADIO, static_cast<int>(embedded_data.size()) + 6, 7);
}

void esp3_sender::send(const types::data &data, const PACKET_TYPE &type,
                       const int &data_length,
                       const std::uint8_t &optional_length) {
  std::vector<unsigned char> to_write;

  // create package
  {
    // sync byte
    to_write.push_back(0x55);

    to_write.push_back((data_length >> 8) & 0xff);
    to_write.push_back(data_length & 0xff);
    to_write.push_back(optional_length);
    // type
    to_write.push_back(type);

    // header crc
    const std::uint8_t crc_header =
        wolf::esp3_crc::compute_crc8(to_write.begin() + 1, to_write.end());
    to_write.push_back(crc_header);
    // data
    std::copy(data.cbegin(), data.cend(), std::back_inserter(to_write));

    const std::uint8_t crc_data =
        wolf::esp3_crc::compute_crc8(data.cbegin(), data.cend());
    to_write.push_back(crc_data);
  }

  std::stringstream bytes;
  for (const auto &byte : to_write)
    bytes << " " << std::hex << static_cast<int>(byte);

  bytes.flush();
  LOGGING_LOG(m_logger, severity::verbose) << "sending bytes: " << bytes.str();
  m_serial.write_async(to_write);
}
