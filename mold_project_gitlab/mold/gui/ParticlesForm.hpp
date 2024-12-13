#ifndef MOLD_GUI_PARTICLESFORM_HPP
#define MOLD_GUI_PARTICLESFORM_HPP

#include <QWidget>
#include <memory>
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/DisplayPowerHandler.hpp"
#include "mold/gui/GsmInformationReader.hpp"
#include "mold/gui/RoomHeaderForm.hpp"
#include "mold/gui/StatusBarForm.hpp"
#include "mold/gui/StatusFormBase.hpp"

namespace Ui {
class ParticlesForm;
}

namespace mold {

class ParticlesForm : public StatusFormBase {
  Q_OBJECT

 public:
  using sensors = std::vector<wolf::sensor_id>;

  explicit ParticlesForm(statusForm::constructorItems &items,
                         const sensors &sensor_ids, QWidget *parent = nullptr);
  ~ParticlesForm() override;

  StatusBarForm *getStatusBar() const override;
  RoomHeaderForm *getRoomHeader() const override;

 private:
  void updateOutput();

  using values = std::array<boost::optional<wolf::sensor_value>, 3>;
  values getValues() const;

 private:
  mutable logging::logger m_logger;

  std::unique_ptr<Ui::ParticlesForm> ui;

  const DataExchange &m_dataExchange;
  const sensors m_sensor_ids;

  using duration = std::chrono::system_clock::duration;
  const duration m_sensor_grey_time;
};
}  // namespace mold

#endif  // MOLD_GUI_PARTICLESFORM_HPP
