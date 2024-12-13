#ifndef WOLF_ESP3_PARSER_HPP
#define WOLF_ESP3_PARSER_HPP

#include "log/logger.hpp"
#include "serial_interface.hpp"

#include <chrono>

namespace wolf {

class esp3_parser : boost::noncopyable {
 public:
  using data = types::data;
  using _1bs = std::uint8_t;
  using _4bs = std::array<std::uint8_t, 4>;
  using eep = std::array<std::uint8_t, 3>;

  esp3_parser();

  void handle_data(const serial_interface::read_buffer& bytes_read,
                   const std::size_t& bytes_transferred);

  boost::signals2::signal<void(const data& data_, const data& optional)>
      signal_telegram;

  boost::signals2::signal<void(const types::id_esp3& id, const data& data,
                               const int rssi)>
      signal_data_4bs;
  boost::signals2::signal<void(const types::id_esp3& id, const data& eep,
                               const int rssi)>
      signal_data_learned;
  boost::signals2::signal<void(const types::id_esp3& id, const data& data,
                               const int rssi)>
      signal_data_vld;
  boost::signals2::signal<void(const data& data_, const data& optional_data)>
      signal_response;

  boost::signals2::signal<void(const types::id_esp3&, const std::uint8_t& r_org,
                               const std::uint8_t& func,
                               const std::uint8_t& type)>
      signal_learned;

 private:
  void parse_buffer();
  void parse_packet(std::uint8_t packet_type, const data& data_,
                    const data& optional_data);
  void parse_type_telegram(const data& data_, const data& optional_data);
  void parse_rps(const types::id_esp3& sender_id, const _1bs& to_parse);
  void parse_4bs(const types::id_esp3& sender_id, const int dBm,
                 const _4bs& to_parse);
  void parse_vld(const types::id_esp3& sender_id, const int rssi,
                 const types::data& to_parse);

  void parse_type_response(const data& data_, const data& optional_data);

  void handle_error();
  void handle_crc_error();
  void handle_data_again();

 private:
  logging::logger m_logger;

  data m_buffer;
  eep m_eep;
  bool m_sync_error;
};
}  // namespace wolf

#endif  // WOLF_ESP3_PARSER_HPP
