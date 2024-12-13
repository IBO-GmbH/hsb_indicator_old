#ifndef MOLD_GUI_NUMBERGENERIC_HPP
#define MOLD_GUI_NUMBERGENERIC_HPP

#include <QWidget>

#include "NumberBase.hpp"
#include "log/logger.hpp"

namespace Ui {
class NumberGeneric;
}

namespace mold {

class NumberGeneric : public NumberBase {
  Q_OBJECT

 public:
  explicit NumberGeneric(const std::string &desciption, const std::string &unit,
                         QWidget *parent = nullptr);
  ~NumberGeneric() override;

  void setValue(const float value) override;
  void adjustSizeCo2();
  void adjustSizeParticlesAndCurrent();

 private:
  void adjustWidgetSize();

 private:
  std::unique_ptr<Ui::NumberGeneric> ui;

  logging::logger m_logger;
};
}  // namespace mold

#endif  // MOLD_GUI_NUMBERGENERIC_HPP
