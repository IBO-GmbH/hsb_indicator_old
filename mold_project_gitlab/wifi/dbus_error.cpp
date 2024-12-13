#include "dbus_error.hpp"
#include <QDBusError>

using namespace wifi;

dbus_error::dbus_error(const QDBusError& error)
    : std::runtime_error(error.message().toStdString()) {}
