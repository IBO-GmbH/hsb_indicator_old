#include "DebugStatusScreen.hpp"
#include "ui_DebugStatusScreen.h"

#include "reset_mode.hpp"
#include "shutdown_handler.hpp"
#include "wolf/hardware_address.hpp"
#include "wolf/url_helper.hpp"

using namespace logging;

mold::DebugStatusScreen::DebugStatusScreen(const WolfServices &services,
                                           QWidget *parent)
    : QWidget(parent),
      ui(new Ui::DebugStatusScreen),
      m_logger("DebugStatusScreen"),
      m_services(services) {
  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &DebugStatusScreen::back);
  connect(ui->shutDown, &QPushButton::clicked, this,
          &DebugStatusScreen::shutDown);
  connect(ui->reboot, &QPushButton::clicked, this, &DebugStatusScreen::reboot);
  connect(ui->reset, &QPushButton::clicked, this, &DebugStatusScreen::reset);
  auto url = wolf::url_helper::combine_url_with_device_id(
      m_services.config_wolf->network_.url,
      m_services.config_wolf->general_.mac_address);
  auto found = url.find_first_of("?");
  if (found != std::string::npos) url.insert(found, "\n");
  ui->serverURL->setText(QString::fromStdString(url));
  getIPAddresses();
}

void mold::DebugStatusScreen::shutDown() { shutdown_handler::shutdown(); }

void mold::DebugStatusScreen::reboot() { shutdown_handler::reboot(); }

void mold::DebugStatusScreen::reset() {
  m_services.reset_handler->reset_config(reset_mode::reboot);
}

void mold::DebugStatusScreen::getIPAddresses() {
  auto addresses = wolf::hardware_address::get_ip_addresses();
  for (auto &address : addresses)
    ui->ipAddresses->addItem(QString(address.data()));
}

mold::DebugStatusScreen::~DebugStatusScreen() = default;
