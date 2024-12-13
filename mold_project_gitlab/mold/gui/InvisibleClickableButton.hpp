#ifndef MOLD_GUI_INVISIBLECLICKABLEBUTTON_HPP
#define MOLD_GUI_INVISIBLECLICKABLEBUTTON_HPP

#include <QPushButton>

namespace mold {

class InvisibleClickableButton : public QPushButton {
 public:
  InvisibleClickableButton(QWidget* parent = nullptr);

  // QWidget interface
 protected:
  void paintEvent(QPaintEvent*);
};
}

#endif  // MOLD_GUI_INVISIBLECLICKABLEBUTTON_HPP
