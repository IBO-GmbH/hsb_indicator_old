#ifndef MOLD_GUI_POSTTOGUITHREAD_HPP
#define MOLD_GUI_POSTTOGUITHREAD_HPP

#include <QCoreApplication>
#include <boost/assert.hpp>

namespace mold {

template <typename Callable>
static void postToGUIThread(Callable &&fun, QObject *target = qApp) {
  struct Event : public QEvent {
    Callable m_fun;

    Event(Callable &&fun)
        : QEvent(QEvent::None), m_fun(std::forward<Callable>(fun)) {
      ;
    }
    ~Event() { m_fun(); }
  };
  BOOST_ASSERT(target != nullptr);
  QCoreApplication::postEvent(target, new Event(std::forward<Callable>(fun)));
}
}  // namespace mold

#endif  // MOLD_GUI_POSTTOGUITHREAD_HPP
