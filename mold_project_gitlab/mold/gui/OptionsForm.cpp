#include "OptionsForm.hpp"
#include "mold/shutdown_handler.hpp"
#include "ui_OptionsForm.h"
#include "wolf/thread_checker.hpp"

using namespace mold;

OptionsForm::OptionsForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::OptionsForm) {
  wolf::thread_checker::throw_if_not_ui("OptionsForm::OptionsForm");

  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &OptionsForm::back);
  connect(ui->displayOptions, &QPushButton::clicked, this,
          &OptionsForm::openDisplayOptions);
  connect(ui->frsiOptions, &QPushButton::clicked, this,
          &OptionsForm::openFrsiOptions);
  connect(ui->resetOptions, &QPushButton::clicked, this,
          &OptionsForm::openResetOptions);
  connect(ui->shutdown, &QPushButton::clicked, this, [this] {
    ui->popUp->setVisible(true);
    ui->options->setEnabled(false);
  });
  connect(ui->popUp, &Confirmation::buttonLeft, this, [this] {
    ui->popUp->setVisible(false);
    ui->options->setEnabled(true);
  });
  connect(ui->popUp, &Confirmation::buttonRight, this,
          [] { mold::shutdown_handler::shutdown(); });
  preparePopUp();
}

OptionsForm::~OptionsForm() = default;

void OptionsForm::preparePopUp() {
  ui->popUp->initialise(tr("Do you really want to shutdown the device?"),
                        tr("No"), tr("Yes"));
  const auto position_x = (width() - ui->popUp->width()) / 2;
  const auto position_y =
      (height() + ui->settingLabel->height() - ui->popUp->height()) / 2;
  ui->popUp->move(position_x, position_y);
  ui->popUp->setVisible(false);
}
