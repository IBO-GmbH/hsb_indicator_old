#include "ComfortQuestionnaireOptions.hpp"
#include "ButtonDisableHandler.hpp"
#include "PostToGUIThread.hpp"
#include "ui_ComfortQuestionnaireOptions.h"
#include "wolf/thread_checker.hpp"

mold::ComfortQuestionnaireOptions::ComfortQuestionnaireOptions(
    WolfServices &services, QWidget *parent)
    : QWidget(parent),
      m_logger("ComfortQuestionnaireOptions"),
      ui(new Ui::ComfortQuestionnaireOptions),
      m_services(services),
      m_max_pages(questionnaire_options::max_number_of_questionnaires / 3),
      m_current_page(1) {
  wolf::thread_checker::throw_if_not_ui(
      "ComfortQuestionnaireOptions::ComfortQuestionnaireOptions");

  ui->setupUi(this);

  ui->number->setMaximum(questionnaire_options::max_number_of_questionnaires);
  connect(ui->back, &QPushButton::clicked, this,
          [this] { saveOptions([this] { emit back(); }); });
  connect(ui->lower, &QPushButton::clicked, this,
          &ComfortQuestionnaireOptions::handleNumberLower);
  connect(ui->raise, &QPushButton::clicked, this,
          &ComfortQuestionnaireOptions::handleNumberRaise);
  connect(ui->next, &QPushButton::clicked, this,
          &ComfortQuestionnaireOptions::handleNext);
  connect(ui->previous, &QPushButton::clicked, this,
          &ComfortQuestionnaireOptions::handlePrevious);
  connect(ui->lowerTime1, &QPushButton::clicked, this, [this] {
    handleTimeLower(ui->timeEdit1, ui->lowerTime1, ui->raiseTime1);
  });
  connect(ui->raiseTime1, &QPushButton::clicked, this, [this] {
    handleTimeRaise(ui->timeEdit1, ui->lowerTime1, ui->raiseTime1);
  });
  connect(ui->lowerTime2, &QPushButton::clicked, this, [this] {
    handleTimeLower(ui->timeEdit2, ui->lowerTime2, ui->raiseTime2);
  });
  connect(ui->raiseTime2, &QPushButton::clicked, this, [this] {
    handleTimeRaise(ui->timeEdit2, ui->lowerTime2, ui->raiseTime2);
  });
  connect(ui->lowerTime3, &QPushButton::clicked, this, [this] {
    handleTimeLower(ui->timeEdit3, ui->lowerTime3, ui->raiseTime3);
  });
  connect(ui->raiseTime3, &QPushButton::clicked, this, [this] {
    handleTimeRaise(ui->timeEdit3, ui->lowerTime3, ui->raiseTime3);
  });
  connect(ui->enableQuestionnaire, &QCheckBox::toggled, this,
          &ComfortQuestionnaireOptions::enableQuestionnaire);
  connect(ui->startNow, &QPushButton::clicked, this,
          [this] { saveOptions([this] { emit startQuestionnaire(); }); });

  ui->previous->setVisible(false);
  loadOptions();
}

void mold::ComfortQuestionnaireOptions::enableQuestionnaire() {
  // after disabling/enabling view shall start on first page
  m_current_page = 1;
  handleVisible();
}

void mold::ComfortQuestionnaireOptions::handleNumberLower() {
  ButtonDisableHandler::handleLower(ui->number, {ui->lower}, {ui->raise});
  update_max_page_number();
  if (m_current_page > m_max_pages) handlePrevious();
  handleVisible();
  handleTimeLimits();
}

void mold::ComfortQuestionnaireOptions::handleNumberRaise() {
  ButtonDisableHandler::handleRaise(ui->number, {ui->lower}, {ui->raise});
  update_max_page_number();
  handleVisible();
  handleTimeLimits();
}

void mold::ComfortQuestionnaireOptions::handleTimeLower(QDateTimeEdit *time,
                                                        QPushButton *lower,
                                                        QPushButton *raise) {
  ButtonDisableHandler::handleLower(time, {lower}, {raise});
  saveSingleTime(getIndex(time), time);
  handleTimeLimits();
}

void mold::ComfortQuestionnaireOptions::handleTimeRaise(QDateTimeEdit *time,
                                                        QPushButton *lower,
                                                        QPushButton *raise) {
  ButtonDisableHandler::handleRaise(time, {lower}, {raise});
  saveSingleTime(getIndex(time), time);
  handleTimeLimits();
}

void mold::ComfortQuestionnaireOptions::handleNext() {
  ++m_current_page;
  handleTimeLimits();
  handleUIChange();
  handleVisible();
  ButtonDisableHandler::checkAllDisabledTime(makeTimeButtons());
}

void mold::ComfortQuestionnaireOptions::handlePrevious() {
  --m_current_page;
  handleTimeLimits();
  handleUIChange();
  handleVisible();
  ButtonDisableHandler::checkAllDisabledTime(makeTimeButtons());
}

void mold::ComfortQuestionnaireOptions::handleTimeLimits() {
  unsigned int offset = 3 * (m_current_page - 1);
  ui->timeEdit3->setMaximumTime(QTime(21, 59));
  ui->timeEdit1->setMinimumTime(QTime(0, 0));
  if (ui->number->value() > 3) {
    if (m_current_page < m_max_pages)
      ui->timeEdit3->setMaximumTime(maxTime(3 + offset));
    if (m_current_page > 1) ui->timeEdit1->setMinimumTime(minTime(offset - 1));
  }
  ui->timeEdit2->setMinimumTime(minTime(offset));
  ui->timeEdit3->setMinimumTime(minTime(1 + offset));
  auto max_time1 = ui->timeEdit3->maximumTime();
  auto max_time2 = ui->timeEdit3->maximumTime();
  if (ui->number->value() > (1 + offset)) max_time1 = maxTime(1 + offset);
  if (ui->number->value() > (2 + offset)) max_time2 = maxTime(2 + offset);
  ui->timeEdit1->setMaximumTime(max_time1);
  ui->timeEdit2->setMaximumTime(max_time2);
  ButtonDisableHandler::checkAllDisabledTime(makeTimeButtons());
}

void mold::ComfortQuestionnaireOptions::handleVisible() {
  ui->options->setVisible(ui->enableQuestionnaire->isChecked());
  if (ui->enableQuestionnaire->isChecked()) {
    ui->next->setVisible((ui->number->value() >= 4) &&
                         (m_current_page < m_max_pages));
    ui->previous->setVisible((ui->number->value() >= 4) &&
                             (m_current_page > 1));
    ui->labelTime2->setVisible(ui->number->value() >= (m_current_page * 3 - 1));
    ui->changeTime2->setVisible(ui->number->value() >=
                                (m_current_page * 3 - 1));
    ui->labelTime3->setVisible(ui->number->value() >= (m_current_page * 3));
    ui->changeTime3->setVisible(ui->number->value() >= (m_current_page * 3));
  }
}

void mold::ComfortQuestionnaireOptions::saveOptions(
    std::function<void()> callOnFinished) {
  setDisabled(true);
  auto options = getOptionsFromUI();
  if (options == m_loaded_options) {
    callOnFinished();
    return;
  }
  m_services.service->post([this,
                            handler = m_services.comfort_questionnaire_handler,
                            options, callOnFinished]() {
    try {
      handler->set_options(options);
    } catch (std::runtime_error &error) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "failed to set options, error:" << error.what();
    }
    postToGUIThread([callOnFinished] { callOnFinished(); });
  });
}

void mold::ComfortQuestionnaireOptions::loadOptions() {
  setDisabled(true);
  m_services.service->post(
      [this, handler = m_services.comfort_questionnaire_handler] {
        const auto options = handler->get_options();
        postToGUIThread([this, options] {
          m_loaded_options = options;
          ui->enableQuestionnaire->setChecked(options.enabled);
          ui->number->setValue(
              static_cast<double>(options.number_of_questionnaires));
          ui->timeEdit1->setTime(
              QTime(options.hours_time[0], options.minutes_time[0]));
          ui->timeEdit2->setTime(
              QTime(options.hours_time[1], options.minutes_time[1]));
          ui->timeEdit3->setTime(
              QTime(options.hours_time[2], options.minutes_time[2]));
          handleLoadingFinished();
        });
      });
}

mold::questionnaire_options
mold::ComfortQuestionnaireOptions::getOptionsFromUI() const {
  mold::questionnaire_options options;
  options.enabled = ui->enableQuestionnaire->isChecked();
  options.number_of_questionnaires = static_cast<int>(ui->number->value());
  saveAllHours(options.hours_time);
  saveAllMinutes(options.minutes_time);
  return options;
}

void mold::ComfortQuestionnaireOptions::handleLoadingFinished() {
  handleVisible();
  handleTimeLimits();
  ButtonDisableHandler::checkAllDisabledValues(makeValueButtons());
  ButtonDisableHandler::checkAllDisabledTime(makeTimeButtons());
  setDisabled(false);
}

mold::ComfortQuestionnaireOptions::valueButtons
mold::ComfortQuestionnaireOptions::makeValueButtons() const {
  valueButtons valueButtons_;
  valueButtons_.push_back({ui->number, ui->number->minimum(), {ui->lower}});
  valueButtons_.push_back({ui->number, ui->number->maximum(), {ui->raise}});
  return valueButtons_;
}

mold::ComfortQuestionnaireOptions::timeButtons
mold::ComfortQuestionnaireOptions::makeTimeButtons() const {
  timeButtons timeButtons_;
  timeButtons_.push_back(
      {ui->timeEdit1, ui->timeEdit1->minimumTime(), {ui->lowerTime1}});
  timeButtons_.push_back(
      {ui->timeEdit1, ui->timeEdit1->maximumTime(), {ui->raiseTime1}});
  timeButtons_.push_back(
      {ui->timeEdit2, ui->timeEdit2->minimumTime(), {ui->lowerTime2}});
  timeButtons_.push_back(
      {ui->timeEdit2, ui->timeEdit2->maximumTime(), {ui->raiseTime2}});
  timeButtons_.push_back(
      {ui->timeEdit3, ui->timeEdit3->minimumTime(), {ui->lowerTime3}});
  timeButtons_.push_back(
      {ui->timeEdit3, ui->timeEdit3->maximumTime(), {ui->raiseTime3}});
  return timeButtons_;
}

void mold::ComfortQuestionnaireOptions::saveSingleTime(
    const unsigned int index, const QDateTimeEdit *time) {
  m_loaded_options.hours_time[index] = time->time().hour();
  m_loaded_options.minutes_time[index] = time->time().minute();
}

void mold::ComfortQuestionnaireOptions::handleUIChange() {
  const unsigned int page_index = 3 * (m_current_page - 1);
  ui->labelTime1->setText(
      QStringLiteral("Question Time %1").arg(page_index + 1));
  ui->timeEdit1->setTime(QTime(m_loaded_options.hours_time[page_index],
                               m_loaded_options.minutes_time[page_index]));
  ui->labelTime2->setText(
      QStringLiteral("Question Time %1").arg(page_index + 2));
  ui->timeEdit2->setTime(QTime(m_loaded_options.hours_time[page_index + 1],
                               m_loaded_options.minutes_time[page_index + 1]));
  ui->labelTime3->setText(
      QStringLiteral("Question Time %1").arg(page_index + 3));
  ui->timeEdit3->setTime(QTime(m_loaded_options.hours_time[page_index + 2],
                               m_loaded_options.minutes_time[page_index + 2]));
}

void mold::ComfortQuestionnaireOptions::saveAllHours(time_array &hours) const {
  std::copy(m_loaded_options.hours_time.begin(),
            m_loaded_options.hours_time.end(), hours.begin());
}

void mold::ComfortQuestionnaireOptions::saveAllMinutes(
    time_array &minutes) const {
  std::copy(m_loaded_options.minutes_time.begin(),
            m_loaded_options.minutes_time.end(), minutes.begin());
}

QTime mold::ComfortQuestionnaireOptions::maxTime(
    const unsigned int index) const {
  return QTime(m_loaded_options.hours_time[index],
               m_loaded_options.minutes_time[index])
      .addSecs(-60);
}

QTime mold::ComfortQuestionnaireOptions::minTime(
    const unsigned int index) const {
  return QTime(m_loaded_options.hours_time[index],
               m_loaded_options.minutes_time[index])
      .addSecs(60);
}

unsigned int mold::ComfortQuestionnaireOptions::getIndex(
    const QDateTimeEdit *time) const {
  auto index = 3 * (m_current_page - 1);
  if (time == ui->timeEdit2) return index + 1;
  if (time == ui->timeEdit3) return index + 2;
  return index;
}

void mold::ComfortQuestionnaireOptions::update_max_page_number() {
  m_max_pages = static_cast<unsigned int>(std::ceil(ui->number->value() / 3));
}

mold::ComfortQuestionnaireOptions::~ComfortQuestionnaireOptions() = default;
