#ifndef WIFI_DBUS_HELPER_HPP
#define WIFI_DBUS_HELPER_HPP

#include <QDBusPendingReply>
#include <QString>

namespace wifi {

class dbus_helper {
 public:
  dbus_helper();

  static QString network_service();
  template <typename T>
  static T get_value_of_pending_reply(QDBusPendingReply<T> pending_reply) {
    T result = pending_reply.value();
    if (!pending_reply.isError()) return result;
    throw dbus_error(pending_reply.error());
  }
};

}  // namespace wifi

#endif  // WIFI_DBUS_HELPER_HPP
