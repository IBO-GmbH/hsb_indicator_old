#ifndef MOLD_GUI_COMFORTQUESTIONNAIREOPTIONS_HPP
#define MOLD_GUI_COMFORTQUESTIONNAIREOPTIONS_HPP

#include <QTimeEdit>
#include <QWidget>
#include "ValueButton.hpp"
#include "WolfServices.hpp"

namespace Ui {
class ComfortQuestionnaireOptions;
}

namespace mold {

class ComfortQuestionnaireOptions : public QWidget {
  Q_OBJECT

 public:
  explicit ComfortQuestionnaireOptions(WolfServices &services,
                                       QWidget *parent = nullptr);
  ~ComfortQuestionnaireOptions();

 signals:
  void back();
  void startQuestionnaire();

 private slots:
  void enableQuestionnaire();
  void handleNumberLower();
  void handleNumberRaise();
  void handleTimeLower(QDateTimeEdit *time, QPushButton *lower,
                       QPushButton *raise);
  void handleTimeRaise(QDateTimeEdit *time, QPushButton *lower,
                       QPushButton *raise);
  void handleNext();
  void handlePrevious();

 private:
  void handleTimeLimits();
  void handleVisible();
  void saveOptions(std::function<void()> callOnFinished);
  void loadOptions();
  questionnaire_options getOptionsFromUI() const;
  void handleLoadingFinished();
  using valueButtons = std::vector<ValueButton>;
  valueButtons makeValueButtons() const;
  using timeButtons = std::vector<TimeButton>;
  timeButtons makeTimeButtons() const;
  void saveSingleTime(const unsigned int index, QDateTimeEdit const *time);
  void handleUIChange();
  using time_array =
      std::array<int, questionnaire_options::max_number_of_questionnaires>;
  void saveAllHours(time_array &hours) const;
  void saveAllMinutes(time_array &minutes) const;
  QTime maxTime(const unsigned int index) const;
  QTime minTime(const unsigned int index) const;
  unsigned int getIndex(QDateTimeEdit const *time) const;
  void update_max_page_number();

 private:
  mutable logging::logger m_logger;
  std::unique_ptr<Ui::ComfortQuestionnaireOptions> ui;
  WolfServices &m_services;
  questionnaire_options m_loaded_options;

  unsigned int m_max_pages;
  unsigned int m_current_page;
};
}  // namespace mold

#endif  // MOLD_GUI_COMFORTQUESTIONNAIREOPTIONS_HPP
