#ifndef MOLD_GUI_BUTTONDISABLEHANDLER_HPP
#define MOLD_GUI_BUTTONDISABLEHANDLER_HPP

#include <QDateTimeEdit>
#include "ValueButton.hpp"

namespace mold {

class ButtonDisableHandler {
 public:
  using buttons = std::vector<QPushButton *>;
  static void handleLower(QDoubleSpinBox *value, const buttons &lower,
                          const buttons &raise);
  static void handleRaise(QDoubleSpinBox *value, const buttons &lower,
                          const buttons &raise);
  static void handleStep(QDoubleSpinBox *value, const buttons &lower,
                         const buttons &raise, const int steps);
  static void handleLower(QDateTimeEdit *time, const buttons &lower,
                          const buttons &raise);
  static void handleRaise(QDateTimeEdit *time, const buttons &lower,
                          const buttons &raise);
  using valueButtons = std::vector<ValueButton>;
  static void checkAllDisabledValues(const valueButtons &buttons);
  using timeButtons = std::vector<TimeButton>;
  static void checkAllDisabledTime(const timeButtons &buttons);

 private:
  static void handleDisable(const ValueButton &button);
  static void handleDisable(const TimeButton &button);
  static void enableOtherButton(const buttons &buttons_);
};
}  // namespace mold

#endif  // MOLD_GUI_BUTTONDISABLEHANDLER_HPP
