#include "WifiConnectForm.hpp"
#include <QTimer>
#include "ui_WifiConnectForm.h"

using namespace logging;

mold::WifiConnectForm::WifiConnectForm(const access_point &to_connect,
                                       QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WifiConnectForm),
      m_logger("WifiConnectForm"),
      m_visible(false) {
  ui->setupUi(this);
  ui->ssid->setText(QString::fromStdString(to_connect.ssid));

  connect(ui->back, &QPushButton::clicked, this, &WifiConnectForm::back);
  connect(ui->connect, &QPushButton::clicked, this,
          &WifiConnectForm::handleConnect);
  connect(ui->visible, &QPushButton::clicked, this,
          &WifiConnectForm::changeVisible);
  connect(ui->backspace, &QPushButton::clicked, this,
          &WifiConnectForm::backspace);

  connect(ui->forwardsLower, &QPushButton::clicked, this,
          &WifiConnectForm::handleForwardsLowerCase);
  connect(ui->backwardsLower, &QPushButton::clicked, this,
          &WifiConnectForm::handleBackwardsLowerCase);
  connect(ui->lowerCase, &QPushButton::clicked, this,
          &WifiConnectForm::enterLowerCase);

  connect(ui->forwardsUpper, &QPushButton::clicked, this,
          &WifiConnectForm::handleForwardsUpperCase);
  connect(ui->backwardsUpper, &QPushButton::clicked, this,
          &WifiConnectForm::handleBackwardsUpperCase);
  connect(ui->upperCase, &QPushButton::clicked, this,
          &WifiConnectForm::enterUpperCase);

  connect(ui->forwardsNumbers, &QPushButton::clicked, this,
          &WifiConnectForm::handleForwardsNumbers);
  connect(ui->backwardsNumbers, &QPushButton::clicked, this,
          &WifiConnectForm::handleBackwardsNumbers);
  connect(ui->numbers, &QPushButton::clicked, this,
          &WifiConnectForm::enterNumbers);

  connect(ui->forwardsSymbols, &QPushButton::clicked, this,
          &WifiConnectForm::handleForwardsSymbols);
  connect(ui->backwardsSymbols, &QPushButton::clicked, this,
          &WifiConnectForm::handleBackwardsSymbols);
  connect(ui->symbols, &QPushButton::clicked, this,
          &WifiConnectForm::enterSymbols);

  setCharacters();
}

const std::string mold::WifiConnectForm::getPassword() { return m_password; }

void mold::WifiConnectForm::handleConnect() {
  m_password = ui->password->text().toStdString();
  emit connecting();
}

void mold::WifiConnectForm::changeVisible() {
  toggleVisible();
  if (m_visible) {
    ui->password->setEchoMode(QLineEdit::EchoMode::Normal);
    return;
  }
  ui->password->setEchoMode(QLineEdit::EchoMode::Password);
}

void mold::WifiConnectForm::backspace() { ui->password->backspace(); }

void mold::WifiConnectForm::handleForwards(characters::iterator &position,
                                           characters &characters_) {
  position = std::next(position);
  if (position == characters_.end()) position = characters_.begin();
}

void mold::WifiConnectForm::handleBackwards(characters::iterator &position,
                                            characters &characters_) {
  if (position == characters_.begin()) position = characters_.end();
  position = std::prev(position);
}

void mold::WifiConnectForm::enter(const QString &to_enter) {
  ui->password->insert(to_enter);
}

void mold::WifiConnectForm::handleForwardsLowerCase() {
  handleForwards(m_lower_case_position, m_lower_case);
  ui->lowerCase->setText(*m_lower_case_position);
}

void mold::WifiConnectForm::handleBackwardsLowerCase() {
  handleBackwards(m_lower_case_position, m_lower_case);
  ui->lowerCase->setText(*m_lower_case_position);
}

void mold::WifiConnectForm::enterLowerCase() {
  QString to_enter = ui->lowerCase->text();
  enter(to_enter);
}

void mold::WifiConnectForm::handleForwardsUpperCase() {
  handleForwards(m_upper_case_position, m_upper_case);
  ui->upperCase->setText(*m_upper_case_position);
}

void mold::WifiConnectForm::handleBackwardsUpperCase() {
  handleBackwards(m_upper_case_position, m_upper_case);
  ui->upperCase->setText(*m_upper_case_position);
}

void mold::WifiConnectForm::enterUpperCase() {
  QString to_enter = ui->upperCase->text();
  enter(to_enter);
}

void mold::WifiConnectForm::handleForwardsNumbers() {
  handleForwards(m_numbers_position, m_numbers);
  ui->numbers->setText(*m_numbers_position);
}

void mold::WifiConnectForm::handleBackwardsNumbers() {
  handleBackwards(m_numbers_position, m_numbers);
  ui->numbers->setText(*m_numbers_position);
}

void mold::WifiConnectForm::enterNumbers() {
  QString to_enter = ui->numbers->text();
  enter(to_enter);
}

void mold::WifiConnectForm::handleForwardsSymbols() {
  handleForwards(m_symbols_position, m_symbols);
  ui->symbols->setText(*m_symbols_position);
}

void mold::WifiConnectForm::handleBackwardsSymbols() {
  handleBackwards(m_symbols_position, m_symbols);
  ui->symbols->setText(*m_symbols_position);
}

void mold::WifiConnectForm::enterSymbols() {
  QString to_enter = ui->symbols->text();
  enter(to_enter);
}

void mold::WifiConnectForm::toggleVisible() {
  if (!m_visible) {
    m_visible = true;
    return;
  }
  m_visible = false;
}

void mold::WifiConnectForm::setCharacters() {
  setLowerCase();
  setUpperCase();
  setNumbers();
  setSymbols();
}

void mold::WifiConnectForm::setLowerCase() {
  m_lower_case = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
                  "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
                  "u", "v", "w", "x", "y", "z", "ä", "ö", "ü", "ß"};
  m_lower_case_position = m_lower_case.begin();
  ui->lowerCase->setText(*m_lower_case_position);
}

void mold::WifiConnectForm::setUpperCase() {
  m_upper_case = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
                  "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
                  "U", "V", "W", "X", "Y", "Z", "Ä", "Ö", "Ü"};
  m_upper_case_position = m_upper_case.begin();
  ui->upperCase->setText(*m_upper_case_position);
}

void mold::WifiConnectForm::setNumbers() {
  m_numbers = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  m_numbers_position = m_numbers.begin();
  ui->numbers->setText(*m_numbers_position);
}

void mold::WifiConnectForm::setSymbols() {
  m_symbols = {"!", "\"", "§", "$", "%", "&&", "/", "(", ")", "=",
               "?", "{",  "}", "[", "]", "\\", "@", "€", "<", ">",
               "|", "^",  "°", ",", ";", ".",  ":", "µ", "-", "_",
               "´", "`",  "+", "*", "~", "#",  "'"};
  m_symbols_position = m_symbols.begin();
  ui->symbols->setText(*m_symbols_position);
}

mold::WifiConnectForm::~WifiConnectForm() = default;
