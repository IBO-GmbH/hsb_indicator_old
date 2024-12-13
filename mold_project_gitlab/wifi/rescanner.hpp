#ifndef WIFI_RESCANNER_HPP
#define WIFI_RESCANNER_HPP

#include <QProcess>
#include <functional>
#include <future>
#include <string>
#include "log/logger.hpp"

namespace wifi {

class rescanner : QObject {
  Q_OBJECT
 public:
  rescanner(QObject *parent = nullptr);
  ~rescanner() = default;

  void rescan();

 private:
  logging::logger m_logger;
  std::future<void> m_fut;
};

}  // namespace wifi

#endif  // WIFI_CONNECTOR_HPP
