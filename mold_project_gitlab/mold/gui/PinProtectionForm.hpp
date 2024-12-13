#ifndef PINPROTECTIONFORM_HPP
#define PINPROTECTIONFORM_HPP

#include <QWidget>

#include "WolfServices.hpp"

namespace Ui {
class PinProtectionForm;
}

namespace mold {

class PinProtectionForm : public QWidget {
  Q_OBJECT

 public:
  explicit PinProtectionForm(const mold::WolfServices &services,
                             const bool change_pin, QWidget *parent = nullptr);
  bool checkIfEmpty();
  ~PinProtectionForm();

 signals:
  void back();
  void correctPin();

 private slots:
  void save();
  void checkPin();
  void backspace();
  void digitClicked(const int digit);

 private:
  void changeToPinConfirm();
  void checkIfFirstPress();
  void displayCurrentPin();

 private:
  std::unique_ptr<Ui::PinProtectionForm> ui;
  logging::logger m_logger;
  const mold::WolfServices &m_services;
  std::string m_saved_pin;
  bool m_is_change;
  bool m_is_first_press;
};
}  // namespace mold

#endif  // PINPROTECTIONFORM_HPP
