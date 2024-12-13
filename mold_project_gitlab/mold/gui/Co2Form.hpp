#ifndef MOLD_GUI_CO2FORM_HPP
#define MOLD_GUI_CO2FORM_HPP

#include <QWidget>
#include <boost/optional.hpp>
#include <chrono>
#include "WolfServices.hpp"
#include "log/logger.hpp"
#include "mold/calculation_state.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/DisplayPowerHandler.hpp"
#include "mold/gui/GsmInformationReader.hpp"
#include "mold/gui/RoomHeaderForm.hpp"
#include "mold/gui/StatusBarForm.hpp"
#include "mold/gui/StatusFormBase.hpp"
#include "wolf/sensor_id.hpp"

namespace Ui {
class Co2Form;
}

namespace mold {

class Co2Form : public StatusFormBase {
  Q_OBJECT

 public:
  using duration = std::chrono::system_clock::duration;

  struct value {
    float value;
    bool old;
  };

  explicit Co2Form(statusForm::constructorItems &items,
                   const wolf::sensor_id &sensor_id,
                   const Co2StateHandler &co2_state, QWidget *parent = nullptr);
  ~Co2Form() override;

  StatusBarForm *getStatusBar() const override;
  RoomHeaderForm *getRoomHeader() const override;

 private slots:
  void handleNewValue(const wolf::sensor_id &id);

 private:
  void updateOutput();
  void setWarningMessage(const QString &title, const QString &message);
  boost::optional<value> getValue() const;
  void setStyle(const value &value_, const QString &colour);
  QString makeStyleSheetOld(const QString &colour);
  QString makeStyleSheet(const QString &colour);

 private:
  logging::logger m_logger;

  std::unique_ptr<Ui::Co2Form> ui;

  const DataExchange &m_dataExchange;
  const wolf::types::uuid_array m_room_id;
  const wolf::sensor_id m_sensor_id;
  const Co2StateHandler &m_co2_state;

  calculation_state m_state{calculation_state::undefined};

  const duration m_sensor_grey_time;
};
}  // namespace mold

#endif  // MOLD_GUI_CO2FORM_HPP
