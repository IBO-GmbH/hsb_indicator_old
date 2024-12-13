#include "SensorActivity.hpp"

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::SensorActivity &to_print) {
  out << "{room_name: " << to_print.room_name << ", RSSI: " << to_print.rssi
      << ", timestamp: " << std::dec
      << to_print.time.time_since_epoch().count();
  return out << "}";
}
