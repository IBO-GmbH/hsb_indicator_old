#include "ComfortQuestionnaire.hpp"
#include "PostToGUIThread.hpp"
#include "ui_ComfortQuestionnaire.h"
#include "wolf/thread_checker.hpp"

mold::ComfortQuestionnaire::ComfortQuestionnaire(WolfServices &services,
                                                 QWidget *parent)
    : QWidget(parent),
      m_logger("ComfortQuestionnaire"),
      ui(new Ui::ComfortQuestionnaire),
      m_services(services),
      m_question_type(QuestionTypes::start) {
  wolf::thread_checker::throw_if_not_ui(
      "ComfortQuestionnaire::ComfortQuestionnaire");
  ui->setupUi(this);
  createButtonGroup();
  loadLastQuestionnaire();

  ui->answer4->setVisible(false);
  ui->answer5->setVisible(false);
  ui->answer6->setVisible(false);

  connect(&m_button_group, QOverload<int>::of(&QButtonGroup::buttonClicked),
          this, &ComfortQuestionnaire::questionAnswered);
}

void mold::ComfortQuestionnaire::questionAnswered(int id) {
  saveAnswerToQuestionnaire(id);
  if (m_question_type == QuestionTypes::start) {
    if (id == 1) startQuestionnaireLater();
    if (id == 2) deactivateQuestionnaire();
  }
  if (m_question_type == QuestionTypes::check) {
    if (id == 0)
      saveQuestionnaire();
    else if (id == 1)
      discardQuestionnaire();
    else {
      m_question_type = QuestionTypes::gender;
      uncheckAll();
      changeQuestionsAndAnswers();
      return;
    }
  }
  int type_as_int = static_cast<int>(m_question_type);
  ++type_as_int;
  m_question_type = static_cast<QuestionTypes>(type_as_int);
  uncheckAll();
  changeQuestionsAndAnswers();
}

void mold::ComfortQuestionnaire::createButtonGroup() {
  m_button_group.addButton(ui->radioButtonAnswer1, 0);
  m_button_group.addButton(ui->radioButtonAnswer2, 1);
  m_button_group.addButton(ui->radioButtonAnswer3, 2);
  m_button_group.addButton(ui->radioButtonAnswer4, 3);
  m_button_group.addButton(ui->radioButtonAnswer5, 4);
  m_button_group.addButton(ui->radioButtonAnswer6, 5);
  m_button_group.setExclusive(true);
}

void mold::ComfortQuestionnaire::changeQuestionsAndAnswers() {
  switch (m_question_type) {
    case QuestionTypes::gender:
      changeToGender();
      break;
    case QuestionTypes::age:
      changeToAge();
      break;
    case QuestionTypes::weight:
      changeToWeight();
      break;
    case QuestionTypes::height:
      changeToHeight();
      break;
    case QuestionTypes::clothing:
      changeToClothing();
      break;
    case QuestionTypes::activity:
      changeToActivity();
      break;
    case QuestionTypes::temperature:
      changeToTemperature();
      break;
    case QuestionTypes::thermic_comfort:
      changeToThermicComfort();
      break;
    case QuestionTypes::heating:
      changeToHeating();
      break;
    case QuestionTypes::air_quality:
      changeToAirQuality();
      break;
    case QuestionTypes::humidity:
      changeToHumidity();
      break;
    case QuestionTypes::check:
      changeToCheck();
      break;
    default:
      break;
  }
}

void mold::ComfortQuestionnaire::changeToGender() {
  ui->answer3->setVisible(false);
  ui->answersGroup->setTitle(tr("Gender"));
  ui->labelAnswer1->setText(tr("Female"));
  ui->labelAnswer2->setText(tr("Male"));
  if (m_last_gender.has_value()) {
    auto gender = m_last_gender.value();
    switch (gender) {
      case mold::gender::female:
        ui->radioButtonAnswer1->setChecked(true);
        return;
      case mold::gender::male:
        ui->radioButtonAnswer2->setChecked(true);
        return;
    }
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "unrecognized answer in last questionnaire!";
  }
}

void mold::ComfortQuestionnaire::changeToAge() {
  ui->answer3->setVisible(true);
  ui->answer4->setVisible(true);
  ui->answer5->setVisible(true);
  ui->answersGroup->setTitle(tr("Age in years"));
  ui->labelAnswer1->setText("20-30");
  ui->labelAnswer2->setText("31-40");
  ui->labelAnswer3->setText("41-50");
  ui->labelAnswer4->setText("51-60");
  ui->labelAnswer5->setText(tr("Older than 61"));
  if (m_last_age.has_value()) {
    auto age = m_last_age.value();
    switch (age) {
      case mold::age::twenty_to_thirty:
        ui->radioButtonAnswer1->setChecked(true);
        return;
      case mold::age::thirty_one_to_forty:
        ui->radioButtonAnswer2->setChecked(true);
        return;
      case mold::age::forty_one_to_fifty:
        ui->radioButtonAnswer3->setChecked(true);
        return;
      case mold::age::fifty_one_to_sixty:
        ui->radioButtonAnswer4->setChecked(true);
        return;
      case mold::age::older_than_sixty:
        ui->radioButtonAnswer5->setChecked(true);
        return;
    }
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "unrecognized answer in last questionnaire!";
  }
}

void mold::ComfortQuestionnaire::changeToWeight() {
  ui->answersGroup->setTitle(tr("Weight in kg"));
  ui->labelAnswer1->setText(tr("Less than 50"));
  ui->labelAnswer2->setText("50-64");
  ui->labelAnswer3->setText("65-79");
  ui->labelAnswer4->setText("80-100");
  ui->labelAnswer5->setText(tr("More than 100"));
  if (m_last_weight.has_value()) {
    auto weight = m_last_weight.value();
    switch (weight) {
      case mold::weight::less_than_fifty:
        ui->radioButtonAnswer1->setChecked(true);
        return;
      case mold::weight::fifty_to_sixty_four:
        ui->radioButtonAnswer2->setChecked(true);
        return;
      case mold::weight::sixty_five_to_seventy_nine:
        ui->radioButtonAnswer3->setChecked(true);
        return;
      case mold::weight::eighty_to_hundred:
        ui->radioButtonAnswer4->setChecked(true);
        return;
      case mold::weight::more_than_hundred:
        ui->radioButtonAnswer5->setChecked(true);
        return;
    }
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "unrecognized answer in last questionnaire!";
  }
}

void mold::ComfortQuestionnaire::changeToHeight() {
  ui->answersGroup->setTitle(tr("Height in cm"));
  ui->labelAnswer1->setText(tr("Less than 159"));
  ui->labelAnswer2->setText("160-170");
  ui->labelAnswer3->setText("171-180");
  ui->labelAnswer4->setText("181-190");
  ui->labelAnswer5->setText(tr("More than 191"));
  if (m_last_height) {
    auto height = m_last_height.value();
    switch (height) {
      case mold::height::less_than_one_sixty:
        ui->radioButtonAnswer1->setChecked(true);
        return;
      case mold::height::one_sixty_to_one_seventy:
        ui->radioButtonAnswer2->setChecked(true);
        return;
      case mold::height::one_seventy_one_to_one_eighty:
        ui->radioButtonAnswer3->setChecked(true);
        return;
      case mold::height::one_eighty_one_to_one_ninety:
        ui->radioButtonAnswer4->setChecked(true);
        return;
      case mold::height::more_than_one_ninety:
        ui->radioButtonAnswer5->setChecked(true);
        return;
    }
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "unrecognized answer in last questionnaire!";
  }
}

void mold::ComfortQuestionnaire::changeToClothing() {
  ui->answersGroup->setTitle(tr("How are you dressed?"));
  ui->labelAnswer1->setText(tr("Tropical\nclothing 0,3 clo"));
  ui->labelAnswer2->setText(tr("Light summer-\nclothing 0,5 clo"));
  ui->labelAnswer3->setText(tr("Light work-\nclothing 0,8 clo"));
  ui->labelAnswer4->setText(tr("Inside winter-\nclothing 1,0 clo"));
  ui->labelAnswer5->setText(tr("Officeclothing\nwinter 1,2 clo"));
}

void mold::ComfortQuestionnaire::changeToActivity() {
  ui->answersGroup->setTitle(
      tr("What activity did you do in the last 15 Min.?"));
  ui->labelAnswer1->setText(tr("Sitting relaxed"));
  ui->labelAnswer2->setText(tr("Sitting activity\n(office/school)"));
  ui->labelAnswer3->setText(tr("Light movement\n(e.g. shopping)"));
  ui->labelAnswer4->setText(tr("Heavier movem.\n(housework)"));
  ui->labelAnswer5->setText(tr("Exhausting act.\n(running, stairs)"));
}

void mold::ComfortQuestionnaire::changeToTemperature() {
  ui->answersGroup->setTitle(tr("How do you perceive the cur. temperature?"));
  ui->labelAnswer1->setText(tr("Chilly"));
  ui->labelAnswer2->setText(tr("Slightly chilly"));
  ui->labelAnswer3->setText(tr("Exactly right"));
  ui->labelAnswer4->setText(tr("Slightly warm"));
  ui->labelAnswer5->setText(tr("Warm"));
  changeColours(false);
}

void mold::ComfortQuestionnaire::changeToThermicComfort() {
  ui->answer6->setVisible(true);
  ui->answersGroup->setTitle(tr("How do you perceive the thermic comfort?"));
  ui->labelAnswer1->setText(tr("Very dis-\ncomforting"));
  ui->labelAnswer2->setText(tr("Dis-\ncomforting"));
  ui->labelAnswer3->setText(tr("Rather dis-\ncomforting"));
  ui->labelAnswer4->setText(tr("Rather\ncomforting"));
  ui->labelAnswer5->setText(tr("Comforting"));
  changeColours(true);
}

void mold::ComfortQuestionnaire::changeToHeating() {
  ui->answer6->setVisible(false);
  ui->answersGroup->setTitle(tr("How would you change the therm. condition?"));
  ui->labelAnswer1->setText(tr("Colder"));
  ui->labelAnswer2->setText(tr("Slightly colder"));
  ui->labelAnswer3->setText(tr("Neutral"));
  ui->labelAnswer4->setText(tr("Slightly warmer"));
  ui->labelAnswer5->setText(tr("Warmer"));
  changeColours(false);
}

void mold::ComfortQuestionnaire::changeToAirQuality() {
  ui->answer6->setVisible(false);
  ui->answersGroup->setTitle(
      tr("How do you perceive the current air quality?"));
  ui->labelAnswer1->setText(tr("Stuffy"));
  ui->labelAnswer2->setText(tr("Slightly stuffy"));
  ui->labelAnswer3->setText(tr("Neutral"));
  ui->labelAnswer4->setText(tr("Moderately\nfresh"));
  ui->labelAnswer5->setText(tr("Pleasently\nfresh"));
  changeColours(false);
}

void mold::ComfortQuestionnaire::changeToHumidity() {
  ui->answersGroup->setTitle(tr("How do you perceive the current humidity?"));
  ui->labelAnswer1->setText(tr("Moist"));
  ui->labelAnswer2->setText(tr("Slightly moist"));
  ui->labelAnswer3->setText(tr("Exactly right"));
  ui->labelAnswer4->setText(tr("Slightly dry"));
  ui->labelAnswer5->setText(tr("Dry"));
}

void mold::ComfortQuestionnaire::changeToCheck() {
  ui->answer4->setVisible(false);
  ui->answer5->setVisible(false);
  ui->answersGroup->setTitle(tr("Do you want to save your answers?"));
  ui->labelAnswer1->setText(tr("Yes"));
  ui->labelAnswer2->setText(tr("No"));
  ui->labelAnswer3->setText(tr("Do it again"));
  changeColours(true);
}

void mold::ComfortQuestionnaire::uncheckAll() {
  m_button_group.setExclusive(false);
  ui->radioButtonAnswer1->setChecked(false);
  ui->radioButtonAnswer2->setChecked(false);
  ui->radioButtonAnswer3->setChecked(false);
  ui->radioButtonAnswer4->setChecked(false);
  ui->radioButtonAnswer5->setChecked(false);
  m_button_group.setExclusive(true);
}

void mold::ComfortQuestionnaire::startQuestionnaireLater() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "later was chosen, start questionnaire again in 30 minutes";
  m_services.service->post(
      [handler = m_services.comfort_questionnaire_handler] {
        handler->delay_questionnaire();
      });
  emit back();
}

void mold::ComfortQuestionnaire::deactivateQuestionnaire() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "deactivate was chosen, questionnaire will be deactivated permanently";
  m_services.service->post(
      [handler = m_services.comfort_questionnaire_handler] {
        handler->deactivate_questionnaire();
      });
  emit back();
}

void mold::ComfortQuestionnaire::saveQuestionnaire() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "save questionnaire, answers are right";
  m_questionnaire.timestamp = std::chrono::system_clock::now();
  m_services.service->post([handler = m_services.comfort_questionnaire_handler,
                            questionnaire = m_questionnaire] {
    handler->save_questionnaire(questionnaire);
  });
  emit back();
}

void mold::ComfortQuestionnaire::discardQuestionnaire() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "discard questionnaire, answers are not right";
  m_services.service->post(
      [handler = m_services.comfort_questionnaire_handler] {
        handler->discard_questionnaire();
      });
  emit back();
}

void mold::ComfortQuestionnaire::saveAnswerToQuestionnaire(const int answer) {
  switch (m_question_type) {
    case QuestionTypes::gender:
      m_questionnaire.gender_ = static_cast<gender>(answer);
      break;
    case QuestionTypes::age:
      m_questionnaire.age_ = static_cast<age>(answer);
      break;
    case QuestionTypes::weight:
      m_questionnaire.weight_ = static_cast<weight>(answer);
      break;
    case QuestionTypes::height:
      m_questionnaire.height_ = static_cast<enum height>(answer);
      break;
    case QuestionTypes::clothing:
      m_questionnaire.clothing_ = static_cast<clothing>(answer);
      break;
    case QuestionTypes::activity:
      m_questionnaire.activity_ = static_cast<activity>(answer);
      break;
    case QuestionTypes::temperature:
      m_questionnaire.temperature_ = static_cast<temperature>(answer - 2);
      break;
    case QuestionTypes::thermic_comfort:
      m_questionnaire.thermic_comfort_ =
          static_cast<thermic_comfort>(std::lround(answer - 2.5));
      break;
    case QuestionTypes::air_quality:
      m_questionnaire.air_quality_ = static_cast<air_quality>(answer - 2);
      break;
    case QuestionTypes::humidity:
      m_questionnaire.humidity_ = static_cast<humidity>(answer - 2);
      break;
    default:
      break;
  }
}

void mold::ComfortQuestionnaire::changeColours(const bool start) {
  if (start) {
    changeColoursBackground();
    return;
  }
  changeColoursColourful();
}

void mold::ComfortQuestionnaire::changeColoursColourful() {
  ui->answer1->setStyleSheet(
      "QWidget {"
      "background-color: #bdd6ee;"
      "}");
  ui->answer2->setStyleSheet(
      "QWidget {"
      "background-color: #deeaf6;"
      "}");
  ui->answer3->setStyleSheet(
      "QWidget {"
      "background-color: #92d050;"
      "}");
  ui->answer4->setStyleSheet(
      "QWidget {"
      "background-color: #ffff00;"
      "}");
  ui->answer5->setStyleSheet(
      "QWidget {"
      "background-color: #ffc000;"
      "}");
}

void mold::ComfortQuestionnaire::changeColoursBackground() {
  ui->answer1->setStyleSheet(
      "QWidget {"
      "background-color: #ffffff;"
      "}");
  ui->answer2->setStyleSheet(
      "QWidget {"
      "background-color: #ffffff;"
      "}");
  ui->answer3->setStyleSheet(
      "QWidget {"
      "background-color: #ffffff;"
      "}");
  ui->answer4->setStyleSheet(
      "QWidget {"
      "background-color: #ffffff;"
      "}");
  ui->answer5->setStyleSheet(
      "QWidget {"
      "background-color: #ffffff;"
      "}");
}

void mold::ComfortQuestionnaire::loadLastQuestionnaire() {
  setDisabled(true);
  m_services.service->post(
      [this, handler = m_services.comfort_questionnaire_handler] {
        const auto last_questionnaire = handler->get_last_questionnaire();
        postToGUIThread([this, last_questionnaire] {
          if (last_questionnaire.has_value()) {
            m_last_gender = last_questionnaire->gender_;
            m_last_age = last_questionnaire->age_;
            m_last_weight = last_questionnaire->weight_;
            m_last_height = last_questionnaire->height_;
          }
          setDisabled(false);
        });
      });
}

mold::ComfortQuestionnaire::~ComfortQuestionnaire() = default;
