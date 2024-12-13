#include "ResetOptions.hpp"
#include "reset_mode.hpp"
#include "ui_ResetOptions.h"
#include "wolf/thread_checker.hpp"

mold::ResetOptions::ResetOptions(const WolfServices &services, QWidget *parent)
    : QWidget(parent), ui(new Ui::ResetOptions), m_services(services) {
  wolf::thread_checker::throw_if_not_ui("ResetOptions::ResetOptions");

  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &ResetOptions::back);
  connect(ui->resetValues, &QPushButton::clicked, this,
          &ResetOptions::handleResetValues);
  connect(ui->resetConfig, &QPushButton::clicked, this,
          &ResetOptions::handleResetConfig);
  connect(ui->popUp, &Confirmation::buttonLeft, this, [this] {
    ui->popUp->setVisible(false);
    ui->resetValues->setEnabled(true);
    ui->resetConfig->setEnabled(true);
  });
  ui->popUp->setVisible(false);
}

void mold::ResetOptions::handleResetValues() {
  handlePopUp(
      tr("Do you really want to delete all sensor values, that were not sent "
         "to the server and shutdown the device?"));
  m_connection_right = connect(ui->popUp, &Confirmation::buttonRight, this,
                               [handler = m_services.reset_handler] {
                                 handler->reset_values(reset_mode::shutdown);
                               });
}

void mold::ResetOptions::handleResetConfig() {
  handlePopUp(
      tr("Do you really want to delete all sensor values, that were not sent "
         "to the server and all settings and shutdown the device?"));
  m_connection_right = connect(ui->popUp, &Confirmation::buttonRight, this,
                               [handler = m_services.reset_handler] {
                                 handler->reset_config(reset_mode::shutdown);
                               });
}

void mold::ResetOptions::handlePopUp(const QString &text) {
  disconnect(m_connection_right);
  ui->popUp->initialise(text, tr("No"), tr("Yes"));
  const auto position_x = (width() - ui->popUp->width()) / 2;
  const auto position_y =
      (height() + ui->settingLabel->height() - ui->popUp->height()) / 2;
  ui->popUp->move(position_x, position_y);
  ui->popUp->setVisible(true);
  ui->resetValues->setEnabled(false);
  ui->resetConfig->setEnabled(false);
}

mold::ResetOptions::~ResetOptions() = default;
