#include "Confirmation.hpp"
#include "ui_Confirmation.h"

mold::Confirmation::Confirmation(QWidget *parent)
    : QWidget(parent), ui(new Ui::Confirmation) {
  ui->setupUi(this);

  connect(ui->buttonLeft, &QPushButton::clicked, this,
          &Confirmation::buttonLeft);
  connect(ui->buttonRight, &QPushButton::clicked, this,
          &Confirmation::buttonRight);
}

void mold::Confirmation::initialise(const QString &text,
                                    const QString &buttonLeft,
                                    const QString &buttonRight) {
  ui->text->setText(text);
  ui->buttonLeft->setText(buttonLeft);
  ui->buttonRight->setText(buttonRight);
  ui->text->adjustSize();
  const auto position_y = ui->text->y() + ui->text->height() + 20;
  ui->buttons->move(ui->buttons->x(), position_y);
  const auto new_height = ui->buttons->y() + ui->buttons->height() + 20;
  ui->frame->setGeometry({0, 0, ui->frame->width(), new_height});
  setGeometry({0, 0, width(), new_height});
}

mold::Confirmation::~Confirmation() = default;
