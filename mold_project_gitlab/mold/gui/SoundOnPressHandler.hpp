#ifndef MOLD_GUI_SOUNDONPRESSHANDLER_HPP
#define MOLD_GUI_SOUNDONPRESSHANDLER_HPP

#include "log/logger.hpp"

#include <QObject>

namespace mold {
struct WolfServices;
}

namespace mold {

class SoundOnPressHandler : public QObject {
  Q_OBJECT
 public:
  using objectList = std::vector<QString>;

  SoundOnPressHandler(const WolfServices &services, QObject *parent = nullptr);

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;
  bool isInput(QEvent *event, bool &on);

 private:
  static bool isAction(QObject *obj);
  static objectList makeObjectList();
  static objectList::const_iterator findObjectInList(const objectList &objects,
                                                     QObject *to_find);

 private:
  logging::logger m_logger;
  const WolfServices &m_services;
  bool m_on;
};
}  // namespace mold

#endif  // MOLD_GUI_SOUNDONPRESSHANDLER_HPP
