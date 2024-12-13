#ifndef GSMCONFIRMATIONFORM_HPP
#define GSMCONFIRMATIONFORM_HPP

#include <QWidget>
#include <memory>

class Ui_ConfirmationForm;

namespace mold {

class ConfirmationForm : public QWidget {
  Q_OBJECT

 public:
  explicit ConfirmationForm(QWidget *parent = nullptr);
  ~ConfirmationForm();

 private:
  void englishPressed();
  void germanPressed();
  void confirmPressed();

 private:
  std::unique_ptr<Ui_ConfirmationForm> ui;
};
} 

#endif
