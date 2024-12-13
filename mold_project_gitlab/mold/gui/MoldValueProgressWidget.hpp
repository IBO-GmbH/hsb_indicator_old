#ifndef MOLD_GUI_MOLDVALUEPROGRESSWIDGET_HPP
#define MOLD_GUI_MOLDVALUEPROGRESSWIDGET_HPP

#include <QWidget>
#include <memory>

namespace Ui {
class MoldValueProgressWidget;
}

namespace mold {

class MoldValueProgressWidget : public QWidget {
  Q_OBJECT
 public:
  explicit MoldValueProgressWidget(QWidget* parent = nullptr);
  ~MoldValueProgressWidget();

  void setThresholds(const int yellow, const int red);
  void setValue(const int mold_value);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void paintBar();
  void setPositions(const int mold_value);
  void setPositionProgressLine(const int mold_value);
  void setPositionProgressValue(const int mold_value);
  QPoint calculatePositionLine(const int mold_value) const;
  QPoint calculatePositionValue(const int mold_value) const;
  int calculateProgressPosition(const int mold_value) const;

 private:
  std::unique_ptr<Ui::MoldValueProgressWidget> ui;

  int m_yellow;
  int m_red;
};
}  // namespace mold

#endif  // MOLD_GUI_MOLDVALUEPROGRESSWIDGET_HPP
