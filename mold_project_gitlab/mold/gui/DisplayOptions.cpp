#include "DisplayOptions.hpp"
#include "ui_DisplayOptions.h"
#include "wolf/thread_checker.hpp"

mold::DisplayOptions::DisplayOptions(const mold::WolfServices &services,
                                     const DataExchange &dataExchange,
                                     QWidget *parent)
    : QWidget(parent),
      ui(new Ui::DisplayOptions),
      m_logger("DisplayOptions"),
      m_services(services),
      m_dataExchange(dataExchange) {
  wolf::thread_checker::throw_if_not_ui("DisplayOptions::DisplayOptions");

  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &DisplayOptions::back);
  connect(ui->enableNightmode, &QCheckBox::toggled, this,
          &DisplayOptions::enableNightmode);
  connect(ui->enableRotation, &QCheckBox::toggled, this,
          &DisplayOptions::enableRotation);
  connect(ui->questionnaireOptions, &QPushButton::clicked, this,
          &DisplayOptions::openQuestionnaireOptions);

  checkNightmodeEnabled();
  checkRotationEnabled();
  if (!m_services.config_wolf->use_tone) {
    ui->enableSounds->setChecked(false);
    ui->enableSounds->setEnabled(false);
  } else {
    connect(ui->enableSounds, &QCheckBox::toggled, this,
            &DisplayOptions::enableSound);
    checkSoundEnabled();
  }
}

mold::DisplayOptions::~DisplayOptions() = default;

void mold::DisplayOptions::enableSound() {
  m_services.service->post([this,
                            tone_handler = m_services.tone_enabled_handler,
                            enable = ui->enableSounds->isChecked()]() {
    handleEnableSound(tone_handler, enable);
  });
}

void mold::DisplayOptions::enableNightmode() {
  m_services.service->post([this, handler = m_services.nightmode_handler,
                            enable = ui->enableNightmode->isChecked()]() {
    handleEnableNightmode(handler, enable);
  });
}

void mold::DisplayOptions::enableRotation() {
  m_services.service->post([this, handler = m_services.options_handler,
                            enable = ui->enableRotation->isChecked()]() {
    handleEnableRotation(handler, enable);
  });
}

void mold::DisplayOptions::checkSoundEnabled() {
  ui->enableSounds->setChecked(m_dataExchange.is_tone_enabled());
}

void mold::DisplayOptions::checkNightmodeEnabled() {
  ui->enableNightmode->setChecked(m_dataExchange.is_nightmode_enabled());
}

void mold::DisplayOptions::checkRotationEnabled() {
  ui->enableRotation->setChecked(m_dataExchange.is_rotation_enabled());
}

void mold::DisplayOptions::handleEnableSound(
    mold::tone_enabled_handler *tone_handler, const bool enable) {
  tone_handler->set_enabled(enable);
}

void mold::DisplayOptions::handleEnableNightmode(
    mold::nightmode_handler *handler, const bool enable) {
  try {
    if (enable) {
      handler->enable();
      return;
    }
    handler->disable();
  } catch (std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "failed to enable nightmode, error:" << error.what();
  }
}

void mold::DisplayOptions::handleEnableRotation(mold::options_handler *handler,
                                                const bool enable) {
  handler->set_rotation_enabled(enable);
}
