#ifndef MOLD_GUI_CONFIRMATION_HPP
#define MOLD_GUI_CONFIRMATION_HPP

#include <QWidget>
#include <memory>

namespace Ui {
class Confirmation;
}

namespace mold {

class Confirmation : public QWidget {
  Q_OBJECT

 public:
  explicit Confirmation(QWidget *parent = nullptr);
  ~Confirmation();
  void initialise(const QString &text, const QString &buttonLeft,
                  const QString &buttonRight);
 signals:
  void buttonLeft();
  void buttonRight();

 private:
  std::unique_ptr<Ui::Confirmation> ui;
};
}  // namespace mold

#endif  // MOLD_GUI_CONFIRMATION_HPP
