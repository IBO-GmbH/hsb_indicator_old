#ifndef MOLD_GUI_OPTIONSPERROOMFORM_HPP
#define MOLD_GUI_OPTIONSPERROOMFORM_HPP

#include "WolfServices.hpp"

#include <QWidget>
#include <boost/signals2/connection.hpp>

namespace Ui {
class OptionsPerRoomForm;
}

namespace mold {

class OptionsPerRoomForm : public QWidget {
  Q_OBJECT

 public:
  explicit OptionsPerRoomForm(
      const WolfServices& services,
      const boost::optional<mold_settings> mold_settings,
      const std::string& room_name, QWidget* parent = nullptr);
  ~OptionsPerRoomForm();

  boost::optional<mold_settings> get();

 signals:
  void back();

 private slots:
  void toggled();
  void handleValueLower();
  void handleValueRaise();

 private:
  void setData(const building_type& building, const float& value,
               const substrate_type& substrate);
  void checkDisabled();
  void setRoomName(const QString& room_name);

 private:
  std::unique_ptr<Ui::OptionsPerRoomForm> ui;

  logging::logger m_logger;
  const mold::WolfServices& m_services;
};
}  // namespace mold

#endif  // MOLD_GUI_OPTIONSPERROOMFORM_HPP
