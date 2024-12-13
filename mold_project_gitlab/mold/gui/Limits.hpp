#ifndef MOLD_GUI_LIMITS_HPP
#define MOLD_GUI_LIMITS_HPP

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QWidget>
#include <memory>

#include "ControllerTypes.hpp"
#include "LimitTypes.hpp"
#include "ValueButton.hpp"
#include "WolfServices.hpp"
#include "log/logger.hpp"
#include "mold/gui/DataExchange.hpp"

namespace Ui {
class Limits;
}

namespace mold {

class Limits : public QWidget {
  Q_OBJECT

 public:
  using limits = wolf::controller::limits;
  using minutes = std::chrono::minutes;
  using check_values = std::array<float, 2>;

  explicit Limits(WolfServices &services, DataExchange &dataExchange,
                  const LimitTypes &limit_type, const ControllerTypes &type,
                  const wolf::types::uuid_array &room_id,
                  QWidget *parent = nullptr);
  ~Limits();

 private slots:
  void handleBack();
  void handlePrevious();
  void handleNext();

 signals:
  void back();
  void previous();
  void next();

 private:
  void changeToHumidity();
  void changeToTemperature();
  void changeToHumidityBorder();
  void changeToSproutValue();
  void changeToVOC();
  void changeToOutdoorTemperature();
  void changeLimitType();
  void setOneInvisible();
  void setLabelsHumidity();
  void setLabelsTemperature();
  void setLabelsHumidityBorder();
  void setLabelsSproutValue();
  void setLabelsVOC();
  void setLabelsOutdoorTemperature();
  void setValuesMax();
  void setValuesDRH();
  void setValuesMin();
  void setValuesReference();
  void setValuesHumidityBorder();
  void setValuesSproutValue();
  void setValuesSproutValueDifference();
  void setValuesVOC();
  void setValuesVOCDifference();
  void setValuesOutdoorTemperature();
  void handleTypeSet();
  void setLimits(std::function<void()> callOnFinished);
  void handleLimitsLoaded();
  void loadLimits();
  void saveOldLimits();
  bool checkIfChanged() const;
  using valueButtons = std::vector<ValueButton>;
  valueButtons makeValueButtons();

 private:
  logging::logger m_logger;

  std::unique_ptr<Ui::Limits> ui;

  WolfServices &m_services;
  DataExchange &m_dataExchange;

  LimitTypes m_type;
  wolf::types::uuid_array m_room_id;

  limits m_limits;
  check_values m_old_limits;
};
}  // namespace mold
#endif  // MOLD_GUI_LIMITS_HPP
