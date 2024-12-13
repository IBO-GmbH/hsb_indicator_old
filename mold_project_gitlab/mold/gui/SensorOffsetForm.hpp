#ifndef MOLD_GUI_SENSOROFFSETFORM_HPP
#define MOLD_GUI_SENSOROFFSETFORM_HPP

#include <QDoubleSpinBox>
#include <QWidget>
#include <memory>

#include "ValueButton.hpp"

namespace Ui {
class SensorOffsetForm;
}

namespace mold {

class SensorOffsetForm : public QWidget {
  Q_OBJECT

 public:
  struct offsets {
    const float temperature;
    const float humidity;
  };
  explicit SensorOffsetForm(const offsets& offsets_, bool show_second,
                            QWidget* parent = nullptr);
  ~SensorOffsetForm();
  offsets getOffset() const;

 signals:
  void back();

 private:
  using valueButtons = std::vector<ValueButton>;
  valueButtons makeValueButtons();
  void checkDisabled();
  void lowerTemperature();
  void lowerHumidity();
  void raiseTemperature();
  void raiseHumidity();
  void stepTemperature(const int steps);
  void stepHumidity(const int steps);

 private:
  std::unique_ptr<Ui::SensorOffsetForm> ui;
};
}  // namespace mold

#endif  // MOLD_GUI_SENSOROFFSETFORM_HPP
