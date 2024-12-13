#ifndef MOLD_GUI_WIFIFORM_HPP
#define MOLD_GUI_WIFIFORM_HPP

#include "DataExchange.hpp"
#include "WifiErrorType.hpp"
#include "wifi/connector.hpp"

#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class WifiForm;
}

namespace mold {

class WifiForm : public QWidget {
  Q_OBJECT

 public:
  using access_point = wifi::access_point;
  using access_points = std::vector<access_point>;
  using optional_access_point = boost::optional<access_point>;

  explicit WifiForm(const mold::WolfServices& services,
                    DataExchange* const dataExchange,
                    const access_point& to_connect, const std::string& password,
                    QWidget* parent = nullptr);
  ~WifiForm();

 signals:
  void back();
  void accessPointChosen(const access_point& chosen);

 public slots:
  void accessPointAdded(access_point added);
  void accessPointRemoved(std::string removed);
  void deviceStateChanged(const wifi::device_state state);
  void didChoose();

 private:
  void makeList();
  void handleUnmanaged();
  void handleUnavailable();
  void handleDisconnected();
  void handleConnecting(const wifi::device_state state);
  void handleNeedAuth();
  void handleConnected();
  void handleDeactivating();
  void handleFailed();
  void handleUnknown();
  void handleSuccessfulConnection(const access_point& connected);
  void handleFailedConnection();
  void handleErrorMessage();
  void handleErrorConfirmed();
  void handleConnect();
  access_points::iterator findAccessPoint(const std::string& path_to_find);
  void setCheckIcon(const QString& ssid);
  void checkConnected();
  boost::optional<QList<QListWidgetItem*>> findItemInList(const QString& ssid);
  void setLockIcon(const access_point& to_check, QListWidgetItem* to_set);
  void handleIconChangeDisconnect();
  void sortAccessPoints();
  bool checkFound(const access_points::iterator& found);
  void changePopUp(const QString& error_text);

 private:
  std::unique_ptr<Ui::WifiForm> ui;

  logging::logger m_logger;

  const WolfServices& m_services;
  DataExchange* const m_dataExchange;

  access_points m_access_points;
  wifi::device_state m_previous_state;
  WifiErrorType m_error;
  QListWidgetItem* m_selected_item;

  const access_point m_to_connect;
  const std::string m_password;
  optional_access_point m_connected;

  std::unique_ptr<wifi::connector> m_connector;
};
}  // namespace mold

#endif  // MOLD_GUI_WIFIFORM_HPP
