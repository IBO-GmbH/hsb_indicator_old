#ifndef DEBUGSTATUSSCREEN_HPP
#define DEBUGSTATUSSCREEN_HPP

#include "WolfServices.hpp"

#include <QWidget>

namespace Ui {
class DebugStatusScreen;
}

namespace mold {

class DebugStatusScreen : public QWidget {
  Q_OBJECT

 public:
  explicit DebugStatusScreen(const WolfServices &services,
                             QWidget *parent = nullptr);
  ~DebugStatusScreen();

 signals:
  void back();

 private:
  void shutDown();
  void reboot();
  void reset();
  void getIPAddresses();

 private:
  std::unique_ptr<Ui::DebugStatusScreen> ui;

  logging::logger m_logger;
  const WolfServices &m_services;
};
}  // namespace mold

#endif  // DEBUGSTATUSSCREEN_HPP
