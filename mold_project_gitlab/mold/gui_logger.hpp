#ifndef WOLF_GUI_LOGGER_HPP
#define WOLF_GUI_LOGGER_HPP

#include "log/logger.hpp"

#include <QtGlobal>
#include <boost/noncopyable.hpp>

namespace mold {

void log_handler(QtMsgType, const QMessageLogContext &, const QString &);

class gui_logger : boost::noncopyable {
 public:
  gui_logger();
  ~gui_logger();

 protected:
  friend void mold::log_handler(QtMsgType, const QMessageLogContext &,
                                const QString &);
  logging::logger m_logger;
};
}

#endif  // WOLF_GUI_LOGGER_HPP
