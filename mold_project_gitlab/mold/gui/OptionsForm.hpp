#ifndef MOLD_GUI_OPTIONSFORM_HPP
#define MOLD_GUI_OPTIONSFORM_HPP

#include <QWidget>
#include <boost/signals2/connection.hpp>
#include "WolfServices.hpp"

namespace Ui {
class OptionsForm;
}

namespace mold {

class OptionsForm : public QWidget {
  Q_OBJECT

 public:
  explicit OptionsForm(QWidget *parent = nullptr);
  ~OptionsForm();

 signals:
  void back();
  void openDisplayOptions();
  void openFrsiOptions();
  void openResetOptions();

 private:
  void preparePopUp();

 private:
  std::unique_ptr<Ui::OptionsForm> ui;
};
}  // namespace mold

#endif  // MOLD_GUI_OPTIONSFORM_HPP
