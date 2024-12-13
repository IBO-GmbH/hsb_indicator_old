#include "NumberHumidity.hpp"
#include "ui_NumberHumidity.h"

using namespace mold;

NumberHumidity::NumberHumidity(QWidget* parent)
    : NumberBase(parent),
      ui(new Ui::NumberHumidity),
      m_logger("NumberHumidity") {
  ui->setupUi(this);
}

NumberHumidity::~NumberHumidity() = default;

void NumberHumidity::setValue(const float value) {
  ui->indoorHumidityLearn->setText(QString::number(static_cast<int>(value)));
}
