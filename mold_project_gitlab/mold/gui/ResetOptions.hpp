#ifndef MOLD_GUI_RESETOPTIONS_HPP
#define MOLD_GUI_RESETOPTIONS_HPP

#include <QWidget>
#include "WolfServices.hpp"

namespace Ui {
class ResetOptions;
}

namespace mold {

class ResetOptions : public QWidget {
  Q_OBJECT

 public:
  explicit ResetOptions(const mold::WolfServices &services,
                        QWidget *parent = nullptr);
  ~ResetOptions();

 signals:
  void back();

 private slots:
  void handleResetValues();
  void handleResetConfig();

 private:
  void handlePopUp(const QString &text);

 private:
  std::unique_ptr<Ui::ResetOptions> ui;

  const mold::WolfServices &m_services;

  QMetaObject::Connection m_connection_right;
};
}  // namespace mold

#endif  // MOLD_GUI_RESETOPTIONS_HPP
