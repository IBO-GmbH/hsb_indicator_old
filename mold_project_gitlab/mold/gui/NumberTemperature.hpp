#ifndef MOLD_GUI_NUMBERTEMPERATURE_HPP
#define MOLD_GUI_NUMBERTEMPERATURE_HPP

#include "NumberBase.hpp"
#include "log/logger.hpp"

namespace Ui {
class NumberTemperature;
}

namespace mold {

class NumberTemperature : public NumberBase {
  Q_OBJECT

 public:
  explicit NumberTemperature(QWidget* parent = nullptr);
  ~NumberTemperature() override;

  void setValue(const float value) override;

 private:
  std::unique_ptr<Ui::NumberTemperature> ui;

  logging::logger m_logger;
};
}

#endif  // MOLD_GUI_NUMBERTEMPERATURE_HPP
