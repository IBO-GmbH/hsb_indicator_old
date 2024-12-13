#ifndef MOLD_GUI_COMFORTQUESTIONNAIRE_HPP
#define MOLD_GUI_COMFORTQUESTIONNAIRE_HPP

#include <QButtonGroup>
#include <QWidget>
#include "ComfortQuestionnaireQuestionTypes.hpp"
#include "DataExchange.hpp"
#include "WolfServices.hpp"

namespace Ui {
class ComfortQuestionnaire;
}

namespace mold {

class ComfortQuestionnaire : public QWidget {
  Q_OBJECT

 public:
  explicit ComfortQuestionnaire(WolfServices &services,
                                QWidget *parent = nullptr);
  ~ComfortQuestionnaire();

 signals:
  void back();

 private slots:
  void questionAnswered(int id);

 private:
  void createButtonGroup();
  void changeQuestionsAndAnswers();
  void changeToGender();
  void changeToAge();
  void changeToWeight();
  void changeToHeight();
  void changeToClothing();
  void changeToActivity();
  void changeToTemperature();
  void changeToThermicComfort();
  void changeToHeating();
  void changeToAirQuality();
  void changeToHumidity();
  void changeToCheck();
  void uncheckAll();
  void startQuestionnaireLater();
  void deactivateQuestionnaire();
  void saveQuestionnaire();
  void discardQuestionnaire();
  void saveAnswerToQuestionnaire(const int answer);
  void changeColours(const bool start);
  void changeColoursColourful();
  void changeColoursBackground();
  void loadLastQuestionnaire();

 private:
  logging::logger m_logger;
  std::unique_ptr<Ui::ComfortQuestionnaire> ui;
  WolfServices &m_services;

  QButtonGroup m_button_group;
  QuestionTypes m_question_type;
  questionnaire m_questionnaire;
  std::optional<mold::gender> m_last_gender;
  std::optional<mold::age> m_last_age;
  std::optional<mold::weight> m_last_weight;
  std::optional<mold::height> m_last_height;
};
}  // namespace mold

#endif  // MOLD_GUI_COMFORTQUESTIONNAIRE_HPP
