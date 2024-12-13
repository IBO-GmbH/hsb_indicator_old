#ifndef WIFI_SCANNER_HPP
#define WIFI_SCANNER_HPP

#include <QObject>
#include <boost/signals2/signal.hpp>
#include <memory>
#include "access_point.hpp"
#include "log/logger.hpp"
#include "org_desktop_networkmanager_fwd.hpp"

class QDBusObjectPath;

namespace wifi {

class scanner : public QObject {
  Q_OBJECT
 public:
  explicit scanner(const std::string& path, QObject* parent = nullptr);
  ~scanner();

  using access_points = std::vector<access_point>;
  const access_points& get_access_points() const;

  boost::signals2::signal<void(access_point)> signal_added;
  boost::signals2::signal<void(std::string path)> signal_removed;

 private slots:
  void handle_access_point_added(const QDBusObjectPath& path);
  void handle_access_point_removed(const QDBusObjectPath& path);

 private:
  void read_access_points();
  access_point read_access_point(const QString& access_point);

  mutable logging::logger m_logger;
  std::unique_ptr<OrgFreedesktopNetworkManagerDeviceWirelessInterface>
      m_interface;
  access_points m_access_points;
};

}  // namespace wifi
#endif  // WIFI_SCANNER_HPP
