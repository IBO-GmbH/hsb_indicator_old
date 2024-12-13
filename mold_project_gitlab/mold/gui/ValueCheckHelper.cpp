#include "ValueCheckHelper.hpp"

bool mold::ValueCheckHelper::isValueValid(
    const mold::ValueCheckHelper::optional_value &value,
    const mold::ValueCheckHelper::timestamp &now,
    const duration &time_reference) {
  if (!value) {
    return false;
  }
  return !checkIfOld(value.get().timestamp_, now, time_reference);
}

bool mold::ValueCheckHelper::checkIfOld(
    const timestamp &to_check, const mold::ValueCheckHelper::timestamp &now,
    const mold::ValueCheckHelper::duration &time_reference) {
  return checkTimeOld(now, to_check, time_reference);
}

void mold::ValueCheckHelper::changeColourIfOld(
    const mold::ValueCheckHelper::labels &to_change, const bool old) {
  for (auto &label : to_change) {
    if (old)
      setGrey(label);
    else
      setDefault(label);
  }
}

void mold::ValueCheckHelper::setNoData(QLabel &number, QLabel *decimal) {
  number.setText("--");
  if (nullptr != decimal) decimal->setText("");
}

bool mold::ValueCheckHelper::checkTimeOld(
    const mold::ValueCheckHelper::timestamp &now,
    const mold::ValueCheckHelper::timestamp &value_time,
    const mold::ValueCheckHelper::duration &to_check) {
  const auto diff = now - value_time;
  if (diff < to_check) {
    return false;
  }
  return true;
}

void mold::ValueCheckHelper::setGrey(QLabel *label) {
  setLabel(label, "color: #d2d2d2");
}

void mold::ValueCheckHelper::setDefault(QLabel *label) { setLabel(label, ""); }

void mold::ValueCheckHelper::setLabel(QLabel *label, const QString &style) {
  label->setStyleSheet(style);
}
