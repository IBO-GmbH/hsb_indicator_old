#include "esp3_parser.hpp"

#include "esp3_crc.hpp"

#include <bitset>

using namespace logging;
using namespace wolf;

esp3_parser::esp3_parser() : m_logger{"esp3_parser"}, m_sync_error(false) {}

void esp3_parser::handle_data(const serial_interface::read_buffer &bytes_read,
                              const std::size_t &bytes_transferred) {
  auto bytes_to_handle = bytes_transferred;
  auto start_byte = bytes_read.cbegin();
  if (m_buffer.empty() || m_sync_error) {
    start_byte =
        std::find_if(bytes_read.cbegin(), bytes_read.cend(),
                     [](const std::uint8_t &byte) { return byte == 0x55; });
    if (start_byte == bytes_read.cend()) {
      LOGGING_LOG(m_logger, severity::warning)
          << "bytes_read packet was invalid, all bytes ignored";
      return;
    }
    bytes_to_handle =
        bytes_transferred -
        static_cast<unsigned int>((start_byte - bytes_read.cbegin()));
    if (start_byte != bytes_read.cbegin())
      LOGGING_LOG(m_logger, severity::warning)
          << "bytes_read did not start with sync_byte, "
          << (bytes_transferred - bytes_to_handle) << " bytes ignored";
    m_sync_error = false;
  }

  std::copy_n(start_byte, bytes_to_handle, std::back_inserter(m_buffer));

  parse_buffer();
}

void esp3_parser::handle_data_again() {
  serial_interface::read_buffer bytes_from_buffer;
  std::copy(m_buffer.begin(), m_buffer.end(), bytes_from_buffer.begin());
  const auto size = m_buffer.size();
  m_buffer.clear();
  handle_data(bytes_from_buffer, size);
}

void esp3_parser::parse_buffer() {
  constexpr std::size_t header_size{6};

  if (m_buffer.size() < header_size) {
    return;
  }
  const std::uint8_t sync_byte{m_buffer[0]};
  const std::size_t data_length{std::size_t(m_buffer[2]) |
                                std::size_t(m_buffer[1]) << 8};
  const std::size_t optional_length{m_buffer[3]};
  const std::uint8_t packet_type{m_buffer[4]};
  const std::uint8_t crc8h{m_buffer[5]};
  const std::size_t packet_length{header_size + data_length + optional_length +
                                  1};  // magic number +1 is crc8d

  if (sync_byte != 0x55) {
    LOGGING_LOG(m_logger, severity::error)
        << "sync_byte != 0x55, sync_byte:" << std::hex
        << static_cast<int>(sync_byte);
    handle_error();
    handle_data_again();
    return;
  }
  // check header-crc!
  const std::uint8_t crc8h_check =
      esp3_crc::compute_crc8(m_buffer.cbegin() + 1, m_buffer.cbegin() + 5);
  if (crc8h != crc8h_check) {
    LOGGING_LOG(m_logger, severity::error)
        << "crc8h != crc8h_check,"
           " crc8h:"
        << std::hex << static_cast<int>(crc8h) << " crc8h_check:" << std::hex
        << static_cast<int>(crc8h_check);
    handle_crc_error();
    return;
  }
  if (m_buffer.size() < packet_length) {
    return;
  }
  const types::data data(
      m_buffer.cbegin() + header_size,
      m_buffer.cbegin() + header_size + static_cast<const int>(data_length));
  const types::data optional_data(
      m_buffer.cbegin() + header_size + static_cast<const int>(data_length),
      m_buffer.cbegin() + header_size + static_cast<const int>(data_length) +
          static_cast<const int>(optional_length));
  const std::uint8_t crc8d{m_buffer[packet_length - 1]};
  // check data-crc! data + optional_data!
  std::uint8_t crc8d_check = esp3_crc::compute_crc8(
      m_buffer.cbegin() + header_size,
      m_buffer.cbegin() + header_size + static_cast<const int>(data_length) +
          static_cast<const int>(optional_length));
  if (crc8d != crc8d_check) {
    LOGGING_LOG(m_logger, severity::error)
        << "crc8d != crc8d_check,"
           " crc8d:"
        << std::hex << static_cast<int>(crc8d) << " crc8d_check:" << std::hex
        << static_cast<int>(crc8d_check);
    handle_crc_error();
    return;
  }

  data::iterator it_till =
      m_buffer.begin() + static_cast<const int>(packet_length);
  m_buffer.erase(m_buffer.begin(), it_till);

  LOGGING_LOG(m_logger, severity::verbose)
      << "parsed header"
      << " sync_byte:0x" << std::hex << static_cast<int>(sync_byte)
      << " data_length:" << std::dec << data_length
      << " optional_length:" << optional_length << " packet_type:0x" << std::hex
      << static_cast<int>(packet_type) << " crc8h:0x" << static_cast<int>(crc8h)
      << " crc8h_check:0x" << static_cast<int>(crc8h_check)
      << " data:" << std::hex << data << " optional_data:" << std::hex
      << optional_data << " crc8d:0x" << static_cast<int>(crc8d);
  signal_telegram(std::cref(data), std::cref(optional_data));

  parse_packet(packet_type, data, optional_data);

  if (!m_buffer.empty()) {
    parse_buffer();
  }
}

void esp3_parser::parse_packet(uint8_t packet_type,
                               const esp3_parser::data &data_,
                               const esp3_parser::data &optional_data) {
  if (packet_type == 0x01) {
    parse_type_telegram(data_, optional_data);
    return;
  }
  if (packet_type == 0x02) {
    parse_type_response(data_, optional_data);
    return;
  }
  parse_type_response(data_, optional_data);
  if (packet_type != 0x01 && packet_type != 0x02) {
    LOGGING_LOG(m_logger, severity::error)
        << "packet_type != 0x01 && packet_type != 0x02, currently only "
           "packet_type 0x01 (telegram) and 0x02 (response) is supported"
        << " packet_type:" << std::hex << static_cast<int>(packet_type);
    return;
  }
}

void esp3_parser::parse_type_telegram(const esp3_parser::data &data_,
                                      const data &optional_data) {
  // parse optional data
  data::const_iterator it_optional_data = optional_data.cbegin();
  const int sub_tel_num{*it_optional_data};
  ++it_optional_data;
  types::id_esp3_array destination;
  std::copy(it_optional_data, it_optional_data + 4, destination.begin());
  it_optional_data += 4;
  const int dBm{-1 * (*it_optional_data)};
  ++it_optional_data;
  const int security_level{*it_optional_data};
  LOGGING_LOG(m_logger, severity::verbose)
      << "parsed optional data, sub_tel_num:" << std::dec << sub_tel_num
      << " destination:" << std::hex << destination << " dBm:" << std::dec
      << dBm << " security_level:" << security_level;
  // parse data
  // for 4bs check enOcean_equipment_profiles_eep2.1.pdf page 9 1.6.2
  // 1byte r_org + 4byte data + 4byte id + 1byte status
  data::const_iterator it_data = data_.cbegin();
  int r_org{*it_data};
  ++it_data;
  if (r_org == 0x07) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "got us a r_org == 0x07, switching it to 0xa5";
    r_org = 0xa5;
  }
  // ute teach in
  if (r_org == 0xd4) {
    // first byte was r_org of ute, check if message is vld, ute has r_org of
    // message as 7th byte (+6)
    r_org = *(it_data + 6);
  }
  if (r_org != 0xa5 && r_org != 0xf6 && r_org != 0xd2) {
    LOGGING_LOG(m_logger, severity::error)
        << "r_org != 0xa5 && r_org != 0xf6 && r_org != 0xd2, currently only "
           "4bs, rps, and vld parsing are implemented, r_org:0x"
        << std::hex << r_org;
    return;
  }

  _4bs four_bs_data;
  _1bs rps_data;
  types::data vld_data;

  if (r_org == 0xa5) {
    std::copy(it_data, it_data + 4, four_bs_data.begin());
    it_data += 4;
  }
  if (r_org == 0xf6) {
    rps_data = *it_data;
    ++it_data;
  }
  if (r_org == 0xd2) {
    // 5: is the size of sender_id + status
    const int size_data = std::distance(it_data, data_.cend() - 5);
    std::copy(it_data, it_data + size_data, std::back_inserter(vld_data));
    it_data += size_data;
  }

  types::id_esp3_array sender_id;
  std::copy(it_data, it_data + 4, sender_id.begin());
  data sender_id_signal_data(sender_id.cbegin(), sender_id.cend());
  types::id_esp3 sender_id_signal =
      static_cast<types::id_esp3>(sender_id[0] << 24 | sender_id[1] << 16 |
                                  sender_id[2] << 8 | sender_id[3]);
  it_data += 4;

  if (r_org == 0xa5) {
    parse_4bs(sender_id_signal, dBm, four_bs_data);
  }
  if (r_org == 0xf6) {
    parse_rps(sender_id_signal, rps_data);
  }
  if (r_org == 0xd2) {
    parse_vld(sender_id_signal, dBm, vld_data);
  }
}

void esp3_parser::parse_rps(const types::id_esp3 &sender_id,
                            const esp3_parser::_1bs &to_parse) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "rps sender_id:" << std::hex << sender_id << " data:" << std::hex
      << static_cast<int>(to_parse);
}

void esp3_parser::parse_4bs(const types::id_esp3 &sender_id, const int dBm,
                            const esp3_parser::_4bs &four_bs_data) {
  int learn_byte{four_bs_data[3]};
  bool we_are_learning{(learn_byte & 0b1000) != 0b1000};
  bool learn_type{(learn_byte & 0b10000000) == 0b10000000};

  LOGGING_LOG(m_logger, severity::verbose)
      << "4bs, learn_byte:"
      << std::bitset<8>(static_cast<unsigned long long>(learn_byte))
      << " sender_id:" << std::hex << sender_id
      << " we_are_learning:" << std::boolalpha << we_are_learning
      << " learn_type:" << learn_type;

  if (we_are_learning) {
    std::bitset<16> to_interpret(static_cast<unsigned long long>(
        (four_bs_data[0] << 8) | four_bs_data[1]));
    std::bitset<16> func_bits =
        (to_interpret >> (3 + 7)) & std::bitset<16>(0b111111);
    std::bitset<16> type_bits =
        (to_interpret >> 3) & std::bitset<16>(0b1111111);
    const unsigned long func{func_bits.to_ulong()};
    const unsigned long type{type_bits.to_ulong()};
    const std::string with_eep = learn_type ? "with_eep" : "WITHOUT_eep";
    LOGGING_LOG(m_logger, severity::normal)
        << "4bs, parsed data LEARNED, r_org:0xa5"
        << " func:" << std::hex << func << " type:" << std::hex << type
        << " sender_id:" << std::hex << sender_id << " with_eep:" << with_eep
        << " to_interpret:" << to_interpret << " func_bits:" << func_bits
        << " type_bits:" << type_bits;
    eep eep_array = {{0xa5, static_cast<std::uint8_t>(func),
                      static_cast<std::uint8_t>(type)}};
    const data eep(eep_array.cbegin(), eep_array.cend());
    signal_data_learned(sender_id, eep, dBm);
    signal_learned(sender_id, eep_array[0], eep_array[1], eep_array[2]);
  } else {
    LOGGING_LOG(m_logger, severity::normal) << "4bs DATA";
    const data data_signal(four_bs_data.cbegin(), four_bs_data.cend());
    signal_data_4bs(sender_id, data_signal, dBm);
  }
}

void esp3_parser::parse_vld(const types::id_esp3 &sender_id, const int rssi,
                            const types::data &to_parse) {
  int learn_byte{to_parse[0]};
  // ute teach in has a data size of 7
  bool we_are_learning{(to_parse.size() == 7) &&
                       ((learn_byte & 0b110000) == 0)};

  LOGGING_LOG(m_logger, severity::verbose)
      << "vld, learn_byte:"
      << std::bitset<8>(static_cast<unsigned long long>(learn_byte))
      << " to_parse.size():" << std::dec << to_parse.size()
      << " sender_id:" << std::hex << sender_id
      << " we_are_learning:" << std::boolalpha << we_are_learning
      << " to_parse:" << to_parse;

  if (we_are_learning) {
    const std::uint8_t func{to_parse[5]};
    const std::uint8_t type{to_parse[4]};
    const std::string teach_in = ((learn_byte & 0b00110000) == 0)
                                     ? "Teach-in request"
                                     : "Teach-in deletion request";
    LOGGING_LOG(m_logger, severity::normal)
        << "vld, parsed data LEARNED, r_org:0xd2" << std::hex
        << "func:" << static_cast<int>(func)
        << " type:" << static_cast<int>(type) << " sender_id:" << sender_id
        << " teach_in:" << teach_in;
    eep eep_array = {{0xd2, func, type}};
    const data eep(eep_array.cbegin(), eep_array.cend());
    signal_data_learned(sender_id, eep, rssi);
    signal_learned(sender_id, eep_array[0], eep_array[1], eep_array[2]);
  } else {
    LOGGING_LOG(m_logger, severity::normal) << "vld DATA";
    signal_data_vld(sender_id, to_parse, rssi);
  }
}

void esp3_parser::parse_type_response(const esp3_parser::data &data_,
                                      const esp3_parser::data &optional_data) {
  signal_response(data_, optional_data);
}

void esp3_parser::handle_error() {
  m_sync_error = true;
  LOGGING_LOG(m_logger, severity::error)
      << "esp3_parser m_sync_error, bytes_read will be ignored till next "
         "sync_byte";
}

void esp3_parser::handle_crc_error() {
  m_buffer.erase(m_buffer.cbegin());
  handle_error();
  handle_data_again();
}
