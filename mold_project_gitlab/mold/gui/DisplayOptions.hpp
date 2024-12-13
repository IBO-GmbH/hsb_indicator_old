#ifndef MOLD_GUI_DISPLAYOPTIONS_HPP
#define MOLD_GUI_DISPLAYOPTIONS_HPP

#include <QWidget>
#include "DataExchange.hpp"
#include "WolfServices.hpp"

namespace Ui {
class DisplayOptions;
}

namespace mold {

class DisplayOptions : public QWidget {
  Q_OBJECT

 public:
  explicit DisplayOptions(const mold::WolfServices &services,
                          const DataExchange &dataExchange,
                          QWidget *parent = nullptr);
  ~DisplayOptions();

 signals:
  void back();
  void openQuestionnaireOptions();

 private slots:
  void enableSound();
  void enableNightmode();
  void enableRotation();

 private:
  void checkSoundEnabled();
  void checkNightmodeEnabled();
  void checkRotationEnabled();
  void handleEnableSound(mold::tone_enabled_handler *tone_handler,
                         const bool enable);
  void handleEnableNightmode(mold::nightmode_handler *handler,
                             const bool enable);
  void handleEnableRotation(mold::options_handler *handler, const bool enable);

 private:
  std::unique_ptr<Ui::DisplayOptions> ui;

  logging::logger m_logger;

  const mold::WolfServices &m_services;
  const mold::DataExchange &m_dataExchange;
};
}  // namespace mold

#endif  // MOLD_GUI_DISPLAYOPTIONS_HPP
