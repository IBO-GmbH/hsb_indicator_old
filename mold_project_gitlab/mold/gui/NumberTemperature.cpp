#include "NumberTemperature.hpp"
#include "ui_NumberTemperature.h"

using namespace mold;

NumberTemperature::NumberTemperature(QWidget* parent)
    : NumberBase(parent),
      ui(new Ui::NumberTemperature),
      m_logger("NumberTemperature") {
  ui->setupUi(this);
}

NumberTemperature::~NumberTemperature() = default;

void NumberTemperature::setValue(const float value) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "setValue, value:" << value;
  const auto old_height = ui->indoorTemperatureLearn->height();
  ui->indoorTemperatureLearn->setText(QString::number(static_cast<int>(value)));
  ui->indoorTemperatureLearn->adjustSize();
  auto geometry = ui->indoorTemperatureLearn->geometry();
  geometry.setHeight(old_height);
  ui->indoorTemperatureLearn->setGeometry(geometry);
  ui->unitDegreeLearn->move(ui->indoorTemperatureLearn->width(),
                            ui->unitDegreeLearn->y());
  adjustSize();
  const auto new_x_icon = width() - 25 - ui->widget_3->width();
  ui->widget_3->move(new_x_icon, ui->widget_3->y());
}
