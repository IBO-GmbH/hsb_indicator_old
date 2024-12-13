#include "dbus_helper.hpp"

using namespace wifi;

dbus_helper::dbus_helper() {}

QString dbus_helper::network_service() {
  static const QString service = "org.freedesktop.NetworkManager";
  return service;
}
