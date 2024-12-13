#include "connector.hpp"

using namespace wifi;
using namespace logging;

connector::connector(const std::string &ssid, const std::string &password,
                     const connector::callback_success &on_success,
                     const connector::callback_failure &on_failure,
                     QObject *parent)
    : QObject(parent), m_logger{"wifi::connector"}, m_on_success{on_success},
      m_on_failure{on_failure} {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "starting a connect to m_ssid:" << ssid;
  connect_async(ssid, password);
}

connector::~connector() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "~dtor waiting for future";
  m_fut.wait();
  LOGGING_LOG(m_logger, logging::severity::verbose) << "~dtor finished";
}

static const std::string s_connection_name = "mold_wifi";

void connector::connect_async(const std::string ssid,
                              const std::string password) {
  m_fut = std::async([this, ssid, password] {
    if (0 != execute("/usr/bin/nmcli", "-f", "GENERAL.STATE", "connection",
                     "show", s_connection_name)) {
      // create connection
      if (0 != execute("/usr/bin/nmcli", "device", "wifi", "connect", ssid,
                       "password", password, "name", s_connection_name)) {
        m_on_failure();
      } else {
        m_on_success();
      }
    }

    execute("/usr/bin/nmcli", "connection", "down", "id", s_connection_name);
    execute("/usr/bin/nmcli", "connection", "modify", "id", s_connection_name,
            "802-11-wireless.ssid", ssid);
    execute("/usr/bin/nmcli", "connection", "modify", "id", s_connection_name,
            "wifi-sec.psk", password);
    execute("/usr/bin/nmcli", "connection", "modify", "id", s_connection_name,
            "connection.autoconnect-retries", "0");

    if (0 != execute("/usr/bin/nmcli", "connection", "up", "id",
                     s_connection_name)) {
      m_on_failure();
    } else {
      m_on_success();
    }
  });
}
