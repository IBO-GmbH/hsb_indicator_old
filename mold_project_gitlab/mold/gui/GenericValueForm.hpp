#ifndef MOLD_GUI_GENERICVALUEFORM_HPP
#define MOLD_GUI_GENERICVALUEFORM_HPP

#include <QWidget>
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/DisplayPowerHandler.hpp"
#include "mold/gui/GsmInformationReader.hpp"
#include "mold/gui/RoomHeaderForm.hpp"
#include "mold/gui/StatusBarForm.hpp"
#include "mold/gui/StatusFormBase.hpp"
#include "mold/gui/WolfServices.hpp"

namespace Ui {
class GenericValueForm;
}

namespace mold {

class GenericValueForm : public StatusFormBase {
  Q_OBJECT

 public:
  using duration = std::chrono::system_clock::duration;

  explicit GenericValueForm(statusForm::constructorItems &items,
                            const wolf::sensor_id &sensor_id,
                            QWidget *parent = nullptr);
  ~GenericValueForm() override;

  StatusBarForm *getStatusBar() const override;
  RoomHeaderForm *getRoomHeader() const override;

 private:
  void updateOutput();
  void setDescriptors();
  boost::optional<float> getValue() const;
  void handleMotion();
  void handleIcon(const wolf::sensor_id &type);

 private:
  logging::logger m_logger;

  std::unique_ptr<Ui::GenericValueForm> ui;

  const WolfServices &m_services;
  const DataExchange &m_dataExchange;

  const wolf::types::uuid_array m_room_id;
  const wolf::sensor_id m_sensor_id;
  const duration m_sensor_grey_time;
};
}  // namespace mold

#endif  // MOLD_GUI_GENERICVALUEFORM_HPP
