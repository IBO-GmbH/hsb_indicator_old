#include "ControllerLabelHandler.hpp"

void mold::ControllerLabelHandler::changeLabelSettings(
    const mold::ControllerTypes &type, QLabel *label) {
  switch (type) {
    case ControllerTypes::sVAS1:
      changeLabel(label, tr("Ventilation"));
      break;
    case ControllerTypes::sVAS2:
      changeLabel(label, tr("Dehumidifier"));
      break;
    case ControllerTypes::sVAS3:
      changeLabel(label, tr("Heating"));
      break;
    case ControllerTypes::sVAS4:
      changeLabel(label, tr("Air Quality"));
      break;
  }
}

void mold::ControllerLabelHandler::changeLabelLimits(
    const mold::ControllerTypes &type, QLabel *label) {
  switch (type) {
    case ControllerTypes::sVAS1:
      changeLabel(label, tr("Ventilation / Limits"));
      break;
    case ControllerTypes::sVAS2:
      changeLabel(label, tr("Dehumidifier / Limits"));
      break;
    case ControllerTypes::sVAS3:
      changeLabel(label, tr("Heating / Limits"));
      break;
    case ControllerTypes::sVAS4:
      changeLabel(label, tr("Air Quality / Limits"));
      break;
  }
}

void mold::ControllerLabelHandler::changeLabelActors(
    const mold::ControllerTypes &type, QLabel *label) {
  switch (type) {
    case ControllerTypes::sVAS1:
      changeLabel(label, tr("Ventil. / Actors"));
      break;
    case ControllerTypes::sVAS2:
      changeLabel(label, tr("Dehum. / Actors"));
      break;
    case ControllerTypes::sVAS3:
      changeLabel(label, tr("Heat. / Actors"));
      break;
    case ControllerTypes::sVAS4:
      changeLabel(label, tr("Air Qu. / Actors"));
      break;
  }
}

void mold::ControllerLabelHandler::changeLabelActorList(
    const mold::ControllerTypes &type, const int index, QLabel *label) {
  const auto new_label_name = getActorListLabel(type, index, label);
  changeLabel(label, new_label_name);
}

void mold::ControllerLabelHandler::changeLabel(QLabel *label,
                                               const QString &new_text) {
  label->setText(new_text);
}

QString mold::ControllerLabelHandler::getActorListLabel(
    const mold::ControllerTypes &type, const int index, QLabel *label) {
  QString new_label_name;
  changeLabelActors(type, label);
  new_label_name = label->text();
  new_label_name += " / ";
  new_label_name += getActorFromIndex(type, index);
  return new_label_name;
}

QString mold::ControllerLabelHandler::getActorFromIndex(
    const mold::ControllerTypes &type, const int index) {
  switch (index) {
    case 0:
      return tr("Ventilator");
    case 1:
      if (type == ControllerTypes::sVAS2) return tr("Dehumid.");
      if (type == ControllerTypes::sVAS3) return tr("Heating");
      return tr("Window 1");
    case 2:
      if (type == ControllerTypes::sVAS2 || type == ControllerTypes::sVAS3)
        return tr("Window 1");
      return tr("Window 2");
    case 3:
      if (type == ControllerTypes::sVAS2 || type == ControllerTypes::sVAS3)
        return tr("Window 2");
      return tr("Window 3");
    default:
      return "invalid";
  }
}
