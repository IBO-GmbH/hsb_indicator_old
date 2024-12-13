#ifndef MOLD_GUI_VALUEBUTTON_HPP
#define MOLD_GUI_VALUEBUTTON_HPP

#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QPushButton>

namespace mold {
struct ValueButton {
  QDoubleSpinBox *value;
  double check_value;
  std::vector<QPushButton *> buttons;
};

struct TimeButton {
  QDateTimeEdit *time;
  QTime check_time;
  std::vector<QPushButton *> buttons;
};
}  // namespace mold
#endif  // MOLD_GUI_VALUEBUTTON_HPP
