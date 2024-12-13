#include "SettingsForm.hpp"
#include "mold/gui/DataExchange.hpp"
#include "ui_SettingsForm.h"

using namespace mold;

SettingsForm::SettingsForm(DataExchange& dataExchange, QWidget* parent)
    : QWidget(parent), ui(new Ui::SettingsForm) {
  ui->setupUi(this);
  connect(ui->back, &QPushButton::clicked, this, &SettingsForm::back);
  connect(ui->changeLanguage, &QPushButton::clicked, this,
          &SettingsForm::language);
  connect(ui->rooms, &QPushButton::clicked, this, &SettingsForm::rooms);
  connect(ui->outdoor, &QPushButton::clicked, this, &SettingsForm::outdoor);
  connect(ui->options, &QPushButton::clicked, this, &SettingsForm::options);
  if (dataExchange.is_wifi_hardware_available())
    connect(ui->wifi, &QPushButton::clicked, this, &SettingsForm::wifi);
  else
    ui->wifi->setDisabled(true);
  connect(ui->activityLog, &QPushButton::clicked, this,
          &SettingsForm::activity);
  connect(ui->changePin, &QPushButton::clicked, this, &SettingsForm::changePin);
}

SettingsForm::~SettingsForm() = default;
