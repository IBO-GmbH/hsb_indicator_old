#ifndef WOLF_ESP3_SENDER_HPP
#define WOLF_ESP3_SENDER_HPP

#include "log/logger.hpp"
#include "types.hpp"

#include <Includes/eoApiDef.h>
#include <boost/noncopyable.hpp>

namespace wolf {
class serial_interface;

class esp3_sender : public boost::noncopyable {
 public:
  esp3_sender(serial_interface& serial);

  void send_rps(const std::uint8_t& send_, const uint8_t &status);
  void send_rps(const std::uint8_t& send_, const types::id_esp3& sender_id, const uint8_t &status);
  void send_telegram(const types::data& data, const RORG_TYPE& org,
                     const types::id_esp3& sender_id, const uint8_t &status);
  void send(const types::data& data, const PACKET_TYPE& type,
            const int& data_length, const uint8_t& optional_data_length);

 private:
 private:
  serial_interface& m_serial;

  logging::logger m_logger;
};
}

#endif  // WOLF_ESP3_SENDER_HPP
