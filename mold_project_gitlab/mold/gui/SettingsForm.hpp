#ifndef MOLD_GUI_SETTINGSFORM_HPP
#define MOLD_GUI_SETTINGSFORM_HPP

#include <QWidget>
#include <memory>

namespace Ui {
class SettingsForm;
}

namespace mold {
struct DataExchange;
}

namespace mold {

class SettingsForm : public QWidget {
  Q_OBJECT

 public:
  explicit SettingsForm(DataExchange& service, QWidget* parent = nullptr);
  ~SettingsForm();

 signals:
  void back();
  void language();
  void rooms();
  void outdoor();
  void options();
  void wifi();
  void activity();
  void changePin();

 private:
  std::unique_ptr<Ui::SettingsForm> ui;
};
}  // namespace mold

#endif  // MOLD_GUI_SETTINGSFORM_HPP
