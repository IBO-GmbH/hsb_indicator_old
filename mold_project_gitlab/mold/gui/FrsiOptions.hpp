#ifndef MOLD_GUI_FRSIOPTIONS_HPP
#define MOLD_GUI_FRSIOPTIONS_HPP

#include <QWidget>
#include "WolfServices.hpp"

namespace Ui {
class FrsiOptions;
}

namespace mold {

class FrsiOptions : public QWidget {
  Q_OBJECT

 public:
  explicit FrsiOptions(const mold::WolfServices &services,
                       QWidget *parent = nullptr);
  ~FrsiOptions();

 signals:
  void back();
  void setPerRoom();

 private slots:
  void save();
  void update();
  void toggled();
  void handleValueLower();
  void handleValueRaise();

 private:
  void setData(const building_type &building, const float &value,
               const substrate_type &substrate);
  void checkDisabled();

 private:
  std::unique_ptr<Ui::FrsiOptions> ui;

  const mold::WolfServices &m_services;

  boost::signals2::scoped_connection m_connection_update_frsi;
  boost::signals2::scoped_connection m_connection_update_substrate;
};
}  // namespace mold

#endif  // MOLD_GUI_FRSIOPTIONS_HPP
