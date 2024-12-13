#ifndef MOLD_GUI_CO2PROGRESSWIDGET_HPP
#define MOLD_GUI_CO2PROGRESSWIDGET_HPP

#include <QWidget>
#include <memory>

namespace Ui {
class Co2ProgressWidget;
}

namespace mold {

class Co2ProgressWidget : public QWidget {
  Q_OBJECT
 public:
  explicit Co2ProgressWidget(QWidget* parent = nullptr);
  ~Co2ProgressWidget();

  void setValue(const float co2);

 private:
  void paintBar();
  void setPositionProgressLine(const float co2);
  QPoint calculatePositionLine(const float co2);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  std::unique_ptr<Ui::Co2ProgressWidget> ui;
};
}  // namespace mold

#endif  // MOLD_GUI_CO2PROGRESSWIDGET_HPP
