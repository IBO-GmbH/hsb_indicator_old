#ifndef MOLD_GUI_DISPLAYPOWERHANDLER_HPP
#define MOLD_GUI_DISPLAYPOWERHANDLER_HPP

#include <QObject>
#include <boost/signals2/signal.hpp>
#include "Co2StateHandler.hpp"
#include "log/logger.hpp"
#include "mold/display_handler.hpp"
#include "mold/gui/DataExchange.hpp"

namespace mold {
struct WolfServices;
}

namespace mold {

class DisplayPowerHandler : public QObject {
  Q_OBJECT
 public:
  DisplayPowerHandler(DataExchange* const dataExchange,
                      const WolfServices& services,
                      const Co2StateHandler& co2_state,
                      QObject* parent = nullptr);

  boost::signals2::signal<void()> signal_there_was_input;

 private slots:
  void handleStatesUpdate();
  void handleOnChanged(const bool on);
  void handleCo2StateChanged(const wolf::types::uuid_array& /*id*/,
                             const calculation_state& state);

 protected:
  bool eventFilter(QObject* obj, QEvent* event) override;
  static bool isInput(QEvent* event);

 private:
  void setOn(const bool on);
  void handleOn(mold::display_handler* handler, const bool on);

 private:
  logging::logger m_logger;

  const DataExchange* const m_dataExchange;
  const WolfServices& m_services;

  bool m_on{true};
};
}  // namespace mold

#endif  // MOLD_GUI_DISPLAYPOWERHANDLER_HPP
