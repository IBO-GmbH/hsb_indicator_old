#include "ConfirmationForm.hpp"
#include <QFile>
#include "gsm_confirmation_file.hpp"
#include "ui_ConfirmationForm.h"

using namespace mold;

ConfirmationForm::ConfirmationForm(QWidget* parent)
    : QWidget(parent), ui(std::make_unique<Ui_ConfirmationForm>()) {
  ui->setupUi(this);

  ui->english->setVisible(false);

  connect(ui->confirm, &QPushButton::clicked, this,
          &ConfirmationForm::confirmPressed);
  connect(ui->english, &QPushButton::clicked, this,
          &ConfirmationForm::englishPressed);
  connect(ui->german, &QPushButton::clicked, this,
          &ConfirmationForm::germanPressed);

  germanPressed();
}

ConfirmationForm::~ConfirmationForm() = default;

void ConfirmationForm::englishPressed() {
  ui->english->setVisible(false);
  ui->german->setVisible(true);
  ui->confirm->setText("Accept");
  ui->label->setText(
      "Umidus No-Return Policy\nI accept the terms and conditions for the use "
      "of "
      "the product Umidus found under www.umidus.com. By accepting these terms "
      "and removing the seal (plastic protection on sensor) I am aware that "
      "the product cannot be returned due to hygiene and health reasons "
      "explicitly stated in the distant vendor law.");
}

void ConfirmationForm::germanPressed() {
  ui->english->setVisible(true);
  ui->german->setVisible(false);
  ui->confirm->setText("Zustimmen");
  ui->label->setText(
      "Umidus Rückgabeausschluss-Klausel\nIch akzeptiere die "
      "Geschäftsbedingungen abrufbar unter www.umidus.com für den Betrieb des "
      "Umidus. Mit der Zustimmung zu diesen Geschäftsbedingungen und dem "
      "Entfernen der Versiegelung (Schutzfolie am Sensor) ist eine Rückgabe "
      "des Produkts nach dem Fernabsatzgesetz aus Hygienegründen und Gründen "
      "des Gesundheitsschutzes ausgeschlossen.");
}

void ConfirmationForm::confirmPressed() {
  gsm_confirmation_file::write_confirmed();
  close();
}
