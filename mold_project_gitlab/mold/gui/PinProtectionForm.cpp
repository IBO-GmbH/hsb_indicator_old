#include "PinProtectionForm.hpp"
#include "ui_PinProtectionForm.h"

mold::PinProtectionForm::PinProtectionForm(const mold::WolfServices &services,
                                           const bool change_pin,
                                           QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PinProtectionForm),
      m_logger("PinProtectionForm"),
      m_services(services),
      m_is_change(change_pin),
      m_is_first_press(true) {
  ui->setupUi(this);
  ui->errorMessage->setVisible(false);

  m_saved_pin = m_services.options_handler->get_pin();

  if (m_is_change) {
    connect(ui->save, &QPushButton::clicked, this, &PinProtectionForm::save);
    displayCurrentPin();
  } else {
    connect(ui->save, &QPushButton::clicked, this,
            &PinProtectionForm::checkPin);
    changeToPinConfirm();
  }

  connect(ui->back, &QPushButton::clicked, this, &PinProtectionForm::back);
  connect(ui->backspace, &QPushButton::clicked, this,
          &PinProtectionForm::backspace);

  connect(ui->digit0, &QPushButton::clicked, this,
          [this]() { digitClicked(0); });
  connect(ui->digit1, &QPushButton::clicked, this,
          [this]() { digitClicked(1); });
  connect(ui->digit2, &QPushButton::clicked, this,
          [this]() { digitClicked(2); });
  connect(ui->digit3, &QPushButton::clicked, this,
          [this]() { digitClicked(3); });
  connect(ui->digit4, &QPushButton::clicked, this,
          [this]() { digitClicked(4); });
  connect(ui->digit5, &QPushButton::clicked, this,
          [this]() { digitClicked(5); });
  connect(ui->digit6, &QPushButton::clicked, this,
          [this]() { digitClicked(6); });
  connect(ui->digit7, &QPushButton::clicked, this,
          [this]() { digitClicked(7); });
  connect(ui->digit8, &QPushButton::clicked, this,
          [this]() { digitClicked(8); });
  connect(ui->digit9, &QPushButton::clicked, this,
          [this]() { digitClicked(9); });
}

void mold::PinProtectionForm::save() {
  std::string pin = ui->pin->text().toStdString();
  if (m_saved_pin == pin) emit back();
  m_saved_pin = pin;
  m_services.service->post(
      [handler = m_services.options_handler, pin]() { handler->set_pin(pin); });
  emit back();
}

void mold::PinProtectionForm::checkPin() {
  std::string pin = ui->pin->text().toStdString();
  if (m_saved_pin == pin) emit correctPin();
  ui->errorMessage->setVisible(true);
  ui->pin->clear();
}

void mold::PinProtectionForm::backspace() {
  checkIfFirstPress();
  ui->pin->backspace();
}

void mold::PinProtectionForm::digitClicked(const int digit) {
  checkIfFirstPress();
  ui->pin->insert(QString::fromStdString(std::to_string(digit)));
}

bool mold::PinProtectionForm::checkIfEmpty() { return m_saved_pin.empty(); }

void mold::PinProtectionForm::changeToPinConfirm() {
  ui->pin->setEchoMode(QLineEdit::EchoMode::Password);
  ui->save->setText("OK");
}

void mold::PinProtectionForm::checkIfFirstPress() {
  if (!m_is_first_press) return;
  ui->pin->clear();
  if (m_is_change) ui->pin->setEchoMode(QLineEdit::EchoMode::Normal);
  m_is_first_press = false;
}

void mold::PinProtectionForm::displayCurrentPin() {
  if (m_saved_pin.empty()) return;
  ui->pin->setEchoMode(QLineEdit::EchoMode::Password);
  ui->pin->setText(QString::fromStdString(m_saved_pin));
}

mold::PinProtectionForm::~PinProtectionForm() = default;
