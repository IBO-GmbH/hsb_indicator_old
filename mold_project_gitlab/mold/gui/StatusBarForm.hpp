#ifndef MOLD_GUI_STATUSBARFORM_HPP
#define MOLD_GUI_STATUSBARFORM_HPP

#include <QWidget>
#include "mold/gui/GsmInformationReader.hpp"
#include "mold/gui/WolfServices.hpp"

class QToolButton;

namespace Ui {
class StatusBarForm;
}

namespace mold {

class StatusBarForm : public QWidget {
  Q_OBJECT

 public:
  explicit StatusBarForm(QWidget *parent = nullptr);
  ~StatusBarForm();

  void initialise(const WolfServices &services,
                  const mold::GsmInformationReader &informationReader);

  QToolButton *getSettingsButton() const;

 public slots:
  void connected();
  void disconnected();
  void blinkForData();

 signals:
  void openSettings();

 private slots:
  void updateTime();
  void setRssi(const std::optional<std::int32_t> rssi);
  void updateGsmProvider(const std::optional<QString> provider);

 private:
  logging::logger m_logger;

  std::unique_ptr<Ui::StatusBarForm> ui;

  boost::signals2::scoped_connection m_connection_connected;
  boost::signals2::scoped_connection m_connection_disconnected;
  boost::signals2::scoped_connection m_blink;
};
}  // namespace mold

#endif  // MOLD_GUI_STATUSBARFORM_HPP
