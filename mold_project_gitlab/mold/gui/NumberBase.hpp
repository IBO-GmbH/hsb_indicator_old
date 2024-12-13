#ifndef MOLD_GUI_NUMBERBASE_HPP
#define MOLD_GUI_NUMBERBASE_HPP

#include <QWidget>

namespace mold {

class NumberBase : public QWidget {
  Q_OBJECT
 public:
  explicit NumberBase(QWidget* parent = nullptr);

  virtual void setValue(const float value) = 0;

 signals:

 public slots:
};
}

#endif  // MOLD_GUI_NUMBERBASE_HPP
