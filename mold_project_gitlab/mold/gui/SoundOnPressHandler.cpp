#include "SoundOnPressHandler.hpp"

#include "mold/gui/WolfServices.hpp"
#include "mold/tone_handler.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QTableWidget>
#include <QToolButton>

using namespace mold;

SoundOnPressHandler::SoundOnPressHandler(const WolfServices &services,
                                         QObject *parent)
    : QObject(parent),
      m_logger{"SoundOnPressHandler"},
      m_services(services),
      m_on{false} {}

bool SoundOnPressHandler::eventFilter(QObject *obj, QEvent *event) {
  bool on;
  if (isInput(event, on) && m_services.tone_handler != nullptr) {
    if (isAction(obj))
      m_services.service->post(
          [handler = m_services.tone_handler]() { handler->start(); });
  }
  return QObject::eventFilter(obj, event);
}

bool SoundOnPressHandler::isInput(QEvent *event, bool &on) {
  switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::MouseButtonPress:
      on = true;
      return true;
    default:
      break;
  }
  return false;
}

bool SoundOnPressHandler::isAction(QObject *obj) {
  if (!obj->isWidgetType()) return false;
  auto widget = dynamic_cast<QWidget *>(obj);
  if (!widget->isEnabled()) return false;
  const static objectList objects = makeObjectList();
  const auto found = findObjectInList(objects, obj);
  if (found != objects.cend()) return true;
  const auto check_widget = QWidget::staticMetaObject.className();
  if (obj->metaObject()->className() == check_widget) {
    auto parent = obj->parent();
    while (parent != nullptr) {
      const auto found_widget = findObjectInList(objects, parent);
      if (found_widget != objects.cend()) return true;
      parent = parent->parent();
    }
  }
  return false;
}

SoundOnPressHandler::objectList SoundOnPressHandler::makeObjectList() {
  objectList objects;
  objects.push_back(QPushButton::staticMetaObject.className());
  objects.push_back(QCheckBox::staticMetaObject.className());
  objects.push_back(QRadioButton::staticMetaObject.className());
  objects.push_back(QLineEdit::staticMetaObject.className());
  objects.push_back(QComboBox::staticMetaObject.className());
  objects.push_back(QToolButton::staticMetaObject.className());
  objects.push_back(QListWidget::staticMetaObject.className());
  objects.push_back(QTableWidget::staticMetaObject.className());
  objects.push_back(QScrollBar::staticMetaObject.className());
  return objects;
}

SoundOnPressHandler::objectList::const_iterator
SoundOnPressHandler::findObjectInList(
    const SoundOnPressHandler::objectList &objects, QObject *to_find) {
  return std::find_if(
      objects.cbegin(), objects.cend(), [to_find](const QString &object_check) {
        return object_check == to_find->metaObject()->className();
      });
}
