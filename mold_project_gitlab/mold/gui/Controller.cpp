#include "Controller.hpp"
#include "ui_Controller.h"

using namespace mold;

Controller::Controller(const wolf::room &room, QWidget *parent)
    : QWidget(parent), ui(new Ui::Controller), m_room(room) {
  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &Controller::back);
  connect(ui->changeSVAS1, &QPushButton::clicked, this,
          &Controller::changeSVAS1);
  connect(ui->changeSVAS2, &QPushButton::clicked, this,
          &Controller::changeSVAS2);
  connect(ui->changeSVAS3, &QPushButton::clicked, this,
          &Controller::changeSVAS3);
  connect(ui->changeSVAS4, &QPushButton::clicked, this,
          &Controller::changeSVAS4);
}

Controller::~Controller() = default;

const wolf::room &Controller::getRoom() const { return m_room; }
