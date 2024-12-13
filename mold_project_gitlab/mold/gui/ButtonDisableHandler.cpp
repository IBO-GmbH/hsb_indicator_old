#include "ButtonDisableHandler.hpp"

#include "wolf/math_compare_decimals.hpp"

void mold::ButtonDisableHandler::handleLower(QDoubleSpinBox *value,
                                             const buttons &lower,
                                             const buttons &raise) {
  value->stepDown();
  enableOtherButton(raise);
  handleDisable({value, value->minimum(), lower});
}

void mold::ButtonDisableHandler::handleRaise(QDoubleSpinBox *value,
                                             const buttons &lower,
                                             const buttons &raise) {
  value->stepUp();
  enableOtherButton(lower);
  handleDisable({value, value->maximum(), raise});
}

void mold::ButtonDisableHandler::handleStep(QDoubleSpinBox *value,
                                            const buttons &lower,
                                            const buttons &raise,
                                            const int steps) {
  const auto abs_steps = std::abs(steps);
  for (auto count = 0; count < abs_steps; ++count) {
    if (steps > 0)
      handleRaise(value, lower, raise);
    else if (steps < 0)
      handleLower(value, lower, raise);
  }
}

void mold::ButtonDisableHandler::handleLower(
    QDateTimeEdit *time, const mold::ButtonDisableHandler::buttons &lower,
    const mold::ButtonDisableHandler::buttons &raise) {
  time->stepDown();
  enableOtherButton(raise);
  handleDisable({time, time->minimumTime(), lower});
}

void mold::ButtonDisableHandler::handleRaise(
    QDateTimeEdit *time, const mold::ButtonDisableHandler::buttons &lower,
    const mold::ButtonDisableHandler::buttons &raise) {
  time->stepUp();
  enableOtherButton(lower);
  handleDisable({time, time->maximumTime(), raise});
}

void mold::ButtonDisableHandler::checkAllDisabledValues(
    const mold::ButtonDisableHandler::valueButtons &buttons) {
  for (const auto &button : buttons) handleDisable(button);
}

void mold::ButtonDisableHandler::checkAllDisabledTime(
    const mold::ButtonDisableHandler::timeButtons &buttons) {
  for (const auto &button : buttons) handleDisable(button);
}

void mold::ButtonDisableHandler::handleDisable(
    const mold::ValueButton &button) {
  for (const auto &button_ : button.buttons) {
    if (wolf::math::compare_doubles(button.value->value(), button.check_value))
      button_->setDisabled(true);
    else
      button_->setDisabled(false);
  }
}

void mold::ButtonDisableHandler::handleDisable(const mold::TimeButton &button) {
  for (const auto &button_ : button.buttons) {
    if (button.time->time() == button.check_time)
      button_->setDisabled(true);
    else
      button_->setDisabled(false);
  }
}

void mold::ButtonDisableHandler::enableOtherButton(const buttons &buttons_) {
  for (const auto &button_ : buttons_) {
    if (button_->isEnabled()) continue;
    button_->setEnabled(true);
  }
}
