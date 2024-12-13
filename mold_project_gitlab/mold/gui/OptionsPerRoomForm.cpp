#include "OptionsPerRoomForm.hpp"
#include "ui_OptionsPerRoomForm.h"

#include "ButtonDisableHandler.hpp"
#include "RoomNameHandler.hpp"
#include "mold/options_handler.hpp"
#include "wolf/thread_checker.hpp"

using namespace mold;

OptionsPerRoomForm::OptionsPerRoomForm(
    const WolfServices &services,
    const boost::optional<mold_settings> mold_settings,
    const std::string &room_name, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::OptionsPerRoomForm),
      m_logger("OptionsPerRoomForm"),
      m_services(services) {
  wolf::thread_checker::throw_if_not_ui(
      "OptionsPerRoomForm::OptionsPerRoomForm");

  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &OptionsPerRoomForm::back);
  connect(ui->oldBuilding, &QRadioButton::toggled, this,
          &OptionsPerRoomForm::toggled);
  connect(ui->newBuilding, &QRadioButton::toggled, this,
          &OptionsPerRoomForm::toggled);
  auto enableUi = [this](const bool &enabled) {
    ui->frsiCustomContainer->setEnabled(enabled);
    ui->newBuilding->setEnabled(enabled);
    ui->oldBuilding->setEnabled(enabled);
    ui->substrateOne->setEnabled(enabled);
    ui->substrateTwo->setEnabled(enabled);
  };
  connect(ui->useCustom, &QCheckBox::toggled, this, enableUi);
  connect(ui->lower, &QPushButton::clicked, this,
          &OptionsPerRoomForm::handleValueLower);
  connect(ui->raise, &QPushButton::clicked, this,
          &OptionsPerRoomForm::handleValueRaise);

  building_type building = m_services.options_handler->get_building_type();
  float value = m_services.options_handler->get_frsi();
  substrate_type substrate = m_services.options_handler->get_substrate_type();
  if (mold_settings) {
    if (mold_settings->building_type_ != building_type::none) {
      building = mold_settings->building_type_;
      value = mold_settings->frsi;
    }
    if (mold_settings->substrate_type_ != substrate_type::none)
      substrate = mold_settings->substrate_type_;
  }
  enableUi(static_cast<bool>(mold_settings));
  ui->useCustom->setChecked(static_cast<bool>(mold_settings));
  setRoomName(QString::fromStdString(room_name));
  setData(building, value, substrate);
}

OptionsPerRoomForm::~OptionsPerRoomForm() = default;

boost::optional<mold_settings> OptionsPerRoomForm::get() {
  if (!ui->useCustom->isChecked()) {
    return {};
  }
  const building_type building = [&] {
    if (ui->oldBuilding->isChecked()) {
      return building_type::old;
    }
    return building_type::new_;
  }();
  const substrate_type substrate = [&] {
    if (ui->substrateOne->isChecked()) {
      return substrate_type::type_one;
    }
    return substrate_type::type_two;
  }();
  const float value = static_cast<const float>(ui->frsi->value());
  return {{building, value, substrate}};
}

void OptionsPerRoomForm::toggled() {
  const float value =
      ui->oldBuilding->isChecked()
          ? m_services.options_handler->get_building_value_old()
          : m_services.options_handler->get_building_value_new();
  ui->frsi->setValue(static_cast<double>(value));
  checkDisabled();
}

void OptionsPerRoomForm::handleValueLower() {
  ButtonDisableHandler::handleLower(ui->frsi, {ui->lower}, {ui->raise});
}

void OptionsPerRoomForm::handleValueRaise() {
  ButtonDisableHandler::handleRaise(ui->frsi, {ui->lower}, {ui->raise});
}

void OptionsPerRoomForm::setData(const building_type &building,
                                 const float &value,
                                 const substrate_type &substrate) {
  switch (building) {
    case building_type::old:
      ui->oldBuilding->setChecked(true);
      break;
    case building_type::new_:
    default:
      ui->newBuilding->setChecked(true);
      break;
  }
  switch (substrate) {
    case substrate_type::type_one:
      ui->substrateOne->setChecked(true);
      break;
    case substrate_type::type_two:
    default:
      ui->substrateTwo->setChecked(true);
      break;
  }
  ui->frsi->setValue(static_cast<double>(value));
  checkDisabled();
}

void OptionsPerRoomForm::checkDisabled() {
  ButtonDisableHandler::checkAllDisabledValues(
      {{ui->frsi, ui->frsi->minimum(), {ui->lower}},
       {ui->frsi, ui->frsi->maximum(), {ui->raise}}});
}

void OptionsPerRoomForm::setRoomName(const QString &room_name) {
  const auto width = ui->settingLabel->width();
  const QString croppedText = RoomNameHandler::getCroppedRoomName(
      ui->settingLabel->font(), width, room_name);
  ui->settingLabel->setText(croppedText);
}
