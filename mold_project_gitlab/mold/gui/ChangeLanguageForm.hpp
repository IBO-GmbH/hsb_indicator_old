#ifndef MOLD_GUI_CHANGELANGUAGEFORM_HPP
#define MOLD_GUI_CHANGELANGUAGEFORM_HPP

#include <QWidget>
#include "mold/gui/LanguageHandler.hpp"
#include "mold/gui/WolfServices.hpp"

namespace Ui {
class ChangeLanguageForm;
}

namespace mold {

class ChangeLanguageForm : public QWidget {
  Q_OBJECT

 public:
  explicit ChangeLanguageForm(const WolfServices& services,
                              LanguageHandler* language,
                              QWidget* parent = nullptr);
  ~ChangeLanguageForm();

 signals:
  void back();
  void debugStatusScreen();

 private slots:
  void pressedAddress();

 private:
  void changeRoomNames(const wolf::languages& language);

 private:
  std::unique_ptr<Ui::ChangeLanguageForm> ui;

  const WolfServices& m_services;

  logging::logger m_logger;
  int m_secretScreenCounter;

  using completion_callback = std::function<void()>;
  completion_callback m_on_completion;
};
}  // namespace mold

#endif  // MOLD_GUI_CHANGELANGUAGEFORM_HPP
