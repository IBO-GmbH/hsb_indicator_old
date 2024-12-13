#ifndef MOLD_GUI_CURRENTCLAMPFORM_HPP
#define MOLD_GUI_CURRENTCLAMPFORM_HPP

#include <QWidget>
#include "mold/gui/StatusFormBase.hpp"

namespace Ui {
class CurrentClampForm;
}

namespace mold {

class CurrentClampForm : public StatusFormBase {
  Q_OBJECT

 public:
  using sensors = std::vector<wolf::sensor_id>;

  explicit CurrentClampForm(statusForm::constructorItems &items,
                            const sensors &sensor_ids,
                            QWidget *parent = nullptr);
  ~CurrentClampForm() override;

  StatusBarForm *getStatusBar() const override;
  RoomHeaderForm *getRoomHeader() const override;

 private:
  void updateOutput();

  using values = std::array<boost::optional<wolf::sensor_value>, 9>;
  values getValues() const;

 private:
  mutable logging::logger m_logger;

  std::unique_ptr<Ui::CurrentClampForm> ui;

  const DataExchange &m_dataExchange;
  const sensors m_sensor_ids;

  using duration = std::chrono::system_clock::duration;
  const duration m_sensor_grey_time;

  std::array<std::optional<float>, 9> m_old_values;
};
}  // namespace mold

#endif  // MOLD_GUI_CURRENTCLAMPFORM_HPP
