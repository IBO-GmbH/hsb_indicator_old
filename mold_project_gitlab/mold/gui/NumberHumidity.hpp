#ifndef MOLD_GUI_NUMBERHUMIDITY_HPP
#define MOLD_GUI_NUMBERHUMIDITY_HPP

#include "NumberBase.hpp"
#include "log/logger.hpp"

namespace Ui {
class NumberHumidity;
}

namespace mold {

class NumberHumidity : public NumberBase {
  Q_OBJECT

 public:
  explicit NumberHumidity(QWidget* parent = nullptr);
  ~NumberHumidity() override;

  void setValue(const float value) override;

 private:
  std::unique_ptr<Ui::NumberHumidity> ui;
  logging::logger m_logger;
};
}

#endif  // MOLD_GUI_NUMBERHUMIDITY_HPP
