#ifndef MOLD_GUI_FOREIGNSENSORFORM_HPP
#define MOLD_GUI_FOREIGNSENSORFORM_HPP

#include <QWidget>
#include <optional>
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/WolfServices.hpp"
#include "wolf/foreign_sensor.hpp"

namespace Ui {
class ForeignSensorForm;
}

namespace mold {

class ForeignSensorForm : public QWidget {
  Q_OBJECT

 public:
  explicit ForeignSensorForm(const mold::WolfServices& services,
                             const mold::DataExchange& dataExchange,
                             QWidget* parent = nullptr);
  ~ForeignSensorForm();
  std::vector<wolf::sensor_profile> get_choosen_profiles() const;

 signals:
  void back();

 private:
  void createListEntries();
  void handleBack();
  struct ListItem;

  std::unique_ptr<Ui::ForeignSensorForm> ui;
  logging::logger m_logger;
  const WolfServices& m_services;
  const DataExchange& m_dataExchange;
};
}  // namespace mold

#endif  // MOLD_GUI_FOREIGNSENSORFORM_HPP
