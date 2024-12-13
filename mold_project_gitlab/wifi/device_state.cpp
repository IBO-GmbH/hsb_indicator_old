#include "device_state.hpp"

std::ostream &wifi::operator<<(std::ostream &out,
                               const wifi::device_state state) {
  switch (state) {
    case device_state::unmanaged:
      return out << "unmanaged";
    case device_state::unavailable:
      return out << "unavailable";
    case device_state::disconnected:
      return out << "disconnected";
    case device_state::prepare:
      return out << "prepare";
    case device_state::config:
      return out << "config";
    case device_state::need_auth:
      return out << "need_auth";
    case device_state::ip_config:
      return out << "ip_config";
    case device_state::ip_check:
      return out << "ip_check";
    case device_state::secondaries:
      return out << "secondaries";
    case device_state::activated:
      return out << "activated";
    case device_state::deactivating:
      return out << "deactivating";
    case device_state::failed:
      return out << "failed";
    case device_state::unknown:
      break;
  }
  return out << "unknown";
}
