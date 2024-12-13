#ifndef MOLD_GUI_CONTROLLERLABELHANDLER_HPP
#define MOLD_GUI_CONTROLLERLABELHANDLER_HPP

#include <QLabel>
#include "ControllerTypes.hpp"

namespace mold {

class ControllerLabelHandler : public QWidget {
  Q_OBJECT
 public:
  static void changeLabelSettings(const ControllerTypes& type, QLabel* label);
  static void changeLabelLimits(const ControllerTypes& type, QLabel* label);
  static void changeLabelActors(const ControllerTypes& type, QLabel* label);
  static void changeLabelActorList(const ControllerTypes& type, const int index,
                                   QLabel* label);

 private:
  static void changeLabel(QLabel* label, const QString& new_text);
  static QString getActorListLabel(const ControllerTypes& type, const int index,
                                   QLabel* label);
  static QString getActorFromIndex(const ControllerTypes& type,
                                   const int index);
};
}  // namespace mold

#endif  // MOLD_GUI_CONTROLLERLABELHANDLER_HPP
