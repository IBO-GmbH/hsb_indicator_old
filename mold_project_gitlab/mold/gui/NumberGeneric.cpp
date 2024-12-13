#include "NumberGeneric.hpp"
#include <cmath>
#include "ui_NumberGeneric.h"

using namespace logging;
using namespace mold;

NumberGeneric::NumberGeneric(const std::string &desciption,
                             const std::string &unit, QWidget *parent)
    : NumberBase(parent), ui(new Ui::NumberGeneric), m_logger("NumberGeneric") {
  ui->setupUi(this);

  LOGGING_LOG(m_logger, severity::verbose)
      << "description:" << desciption << ", unit:" << unit;

  ui->descriptionLearn->setText(QString::fromStdString(desciption));
  ui->unitLearn->setText(QString::fromStdString(unit));
}

NumberGeneric::~NumberGeneric() = default;

void NumberGeneric::setValue(const float value) {
  LOGGING_LOG(m_logger, severity::verbose) << "setValue, value:" << value;

  QLocale local;
  const auto round_value = std::floor(value * 100) / 100;
  ui->genericValueLearn->setText(local.toString(round_value, 'g', 6));
}

void NumberGeneric::adjustSizeCo2() {
  QLocale local;
  ui->genericValueLearn->setText(local.toString(9999));
  setStyleSheet(
      "#genericValueLearn {"
      "font-size: 26px;"
      "}"
      "#descriptionLearn {"
      "font-size: 16px;"
      "}"
      "#unitLearn {"
      "font-size: 14px"
      "}");
  adjustWidgetSize();
}

void NumberGeneric::adjustSizeParticlesAndCurrent() {
  QLocale local;
  ui->genericValueLearn->setText(local.toString(511));
  setStyleSheet(
      "#genericValueLearn {"
      "font-size: 26px;"
      "}"
      "#descriptionLearn {"
      "font-size: 16px;"
      "}"
      "#unitLearn {"
      "font-size: 14px"
      "}");
  adjustWidgetSize();
}

void NumberGeneric::adjustWidgetSize() {
  ui->genericValueLearn->adjustSize();
  auto new_width_value = ui->genericValueLearn->width();
  ui->descriptionLearn->adjustSize();
  auto geometry_descriptionLearn = ui->descriptionLearn->geometry();
  auto new_width_descriptionLearn =
      std::max(geometry_descriptionLearn.width(), new_width_value);
  geometry_descriptionLearn.setWidth(new_width_descriptionLearn);
  ui->descriptionLearn->setGeometry(geometry_descriptionLearn);
  ui->genericValueLearn->move(0, ui->descriptionLearn->height());
  const auto new_position_x_unit = new_width_value + 5;
  ui->unitLearn->move(new_position_x_unit, ui->descriptionLearn->height());
  ui->unitLearn->adjustSize();
  adjustSize();
  ui->genericValueLearn->setText("--");
}
