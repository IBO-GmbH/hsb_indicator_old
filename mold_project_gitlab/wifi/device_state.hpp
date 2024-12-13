#ifndef WIFI_DEVICE_STATE_HPP
#define WIFI_DEVICE_STATE_HPP

#include <QObject>

namespace wifi {

enum class device_state {
  unknown = 0,
  unmanaged = 10,  // wifi device disconnected
  unavailable = 20,
  disconnected = 30,   // disconnect from both sides or wrong password
  prepare = 40,        // connecting
  config = 50,         // connecting
  need_auth = 60,      // wrong password
  ip_config = 70,      // connecting
  ip_check = 80,       // connecting
  secondaries = 90,    // connecting
  activated = 100,     // connected
  deactivating = 110,  // before disconnecting, disconnect from device
  failed = 120         // access point not reachable or wrong password
};

std::ostream &operator<<(std::ostream &out, const device_state state);

}  // namespace wifi

#endif  // WIFI_DEVICE_STATE_HPP
