#ifndef WIFI_DBUS_ERROR_HPP
#define WIFI_DBUS_ERROR_HPP

#include <stdexcept>

class QDBusError;

namespace wifi {

class dbus_error : public std::runtime_error {
 public:
  dbus_error(const QDBusError& error);
};
}  // namespace wifi

#endif  // DBUS_ERROR_HPP
