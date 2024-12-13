#include "InvisibleClickableButton.hpp"

using namespace mold;

InvisibleClickableButton::InvisibleClickableButton(QWidget* parent)
    : QPushButton(parent) {}

void mold::InvisibleClickableButton::paintEvent(QPaintEvent*) {
  // do nothing!
}
