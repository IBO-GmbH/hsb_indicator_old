#ifndef MOLD_WIFICONNECTFORM_HPP
#define MOLD_WIFICONNECTFORM_HPP

#include "LanguageHandler.hpp"
#include "log/logger.hpp"
#include "wifi/access_point.hpp"

#include <QWidget>

namespace Ui {
class WifiConnectForm;
}

namespace mold {

class WifiConnectForm : public QWidget {
  Q_OBJECT

  using access_point = wifi::access_point;
  using characters = std::vector<QString>;

 public:
  explicit WifiConnectForm(const access_point &to_connect,
                           QWidget *parent = nullptr);
  ~WifiConnectForm();
  const std::string getPassword();

 signals:
  void back();
  void connecting();

 private slots:
  void handleConnect();
  void changeVisible();
  void backspace();
  void handleForwards(characters::iterator &position, characters &characters_);
  void handleBackwards(characters::iterator &position, characters &characters_);
  void enter(const QString &to_enter);
  void handleForwardsLowerCase();
  void handleBackwardsLowerCase();
  void enterLowerCase();
  void handleForwardsUpperCase();
  void handleBackwardsUpperCase();
  void enterUpperCase();
  void handleForwardsNumbers();
  void handleBackwardsNumbers();
  void enterNumbers();
  void handleForwardsSymbols();
  void handleBackwardsSymbols();
  void enterSymbols();

 private:
  void toggleVisible();
  void setCharacters();
  void setLowerCase();
  void setUpperCase();
  void setNumbers();
  void setSymbols();

 private:
  std::unique_ptr<Ui::WifiConnectForm> ui;

  logging::logger m_logger;

  std::string m_password;
  bool m_visible;

  characters m_lower_case;
  characters::iterator m_lower_case_position;

  characters m_upper_case;
  characters::iterator m_upper_case_position;

  characters m_numbers;
  characters::iterator m_numbers_position;

  characters m_symbols;
  characters::iterator m_symbols_position;
};
}  // namespace mold
#endif  // MOLD_WIFICONNECTFORM_HPP
