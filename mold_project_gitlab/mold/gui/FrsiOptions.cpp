#include "FrsiOptions.hpp"
#include "ButtonDisableHandler.hpp"
#include "ui_FrsiOptions.h"
#include "wolf/thread_checker.hpp"

mold::FrsiOptions::FrsiOptions(const WolfServices &services, QWidget *parent)
    : QWidget(parent), ui(new Ui::FrsiOptions), m_services(services) {
  wolf::thread_checker::throw_if_not_ui("FrsiOptions::FrsiOptions");

  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &FrsiOptions::save);
  connect(ui->oldBuildingGlobal, &QRadioButton::toggled, this,
          &FrsiOptions::toggled);
  connect(ui->newBuildingGlobal, &QRadioButton::toggled, this,
          &FrsiOptions::toggled);
  connect(ui->setPerRoom, &QPushButton::clicked, this, &FrsiOptions::save);
  connect(ui->setPerRoom, &QPushButton::clicked, this,
          &FrsiOptions::setPerRoom);
  connect(ui->lower, &QPushButton::clicked, this,
          &FrsiOptions::handleValueLower);
  connect(ui->raise, &QPushButton::clicked, this,
          &FrsiOptions::handleValueRaise);

  update();

  m_connection_update_frsi =
      m_services.options_handler->signal_frsi_updated.connect([this]() {
        QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
      });
  m_connection_update_substrate =
      m_services.options_handler->signal_substrate_type_changed.connect(
          [this]() {
            QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
          });
}

mold::FrsiOptions::~FrsiOptions() = default;

void mold::FrsiOptions::save() {
  const auto building = ui->oldBuildingGlobal->isChecked()
                            ? building_type::old
                            : building_type::new_;
  const auto substrate = ui->substrateOneGlobal->isChecked()
                             ? substrate_type::type_one
                             : substrate_type::type_two;
  const float value = static_cast<float>(ui->frsi->value());
  m_services.service->post(
      [handler = m_services.options_handler, building, value, substrate]() {
        handler->set_building_type_and_frsi(building, value);
        handler->set_substrate_type(substrate);
      });
  emit back();
}

void mold::FrsiOptions::update() {
  wolf::thread_checker::throw_if_not_ui("FrsiOptions::update");
  const building_type building =
      m_services.options_handler->get_building_type();
  const float value = m_services.options_handler->get_frsi();
  const substrate_type substrate =
      m_services.options_handler->get_substrate_type();
  setData(building, value, substrate);
}

void mold::FrsiOptions::toggled() {
  const float value =
      ui->oldBuildingGlobal->isChecked()
          ? m_services.options_handler->get_building_value_old()
          : m_services.options_handler->get_building_value_new();
  ui->frsi->setValue(static_cast<double>(value));
  checkDisabled();
}

void mold::FrsiOptions::setData(const building_type &building,
                                const float &value,
                                const substrate_type &substrate) {
  switch (building) {
    case building_type::old:
      ui->oldBuildingGlobal->setChecked(true);
      break;
    case building_type::new_:
    default:
      ui->newBuildingGlobal->setChecked(true);
      break;
  }
  switch (substrate) {
    case substrate_type::type_one:
      ui->substrateOneGlobal->setChecked(true);
      break;
    case substrate_type::type_two:
    default:
      ui->substrateTwoGlobal->setChecked(true);
      break;
  }
  ui->frsi->setValue(static_cast<double>(value));
  checkDisabled();
}

void mold::FrsiOptions::handleValueLower() {
  ButtonDisableHandler::handleLower(ui->frsi, {ui->lower}, {ui->raise});
}

void mold::FrsiOptions::handleValueRaise() {
  ButtonDisableHandler::handleRaise(ui->frsi, {ui->lower}, {ui->raise});
}

void mold::FrsiOptions::checkDisabled() {
  ButtonDisableHandler::checkAllDisabledValues(
      {{ui->frsi, ui->frsi->minimum(), {ui->lower}},
       {ui->frsi, ui->frsi->maximum(), {ui->raise}}});
}
