#include "gui_logger.hpp"

#include <QString>

using namespace logging;

static mold::gui_logger* g_gui_logger;

static severity cast_qtmsgtype_to_severity(QtMsgType type) {
  switch (type) {
    case QtDebugMsg:
      return severity::verbose;
    case QtInfoMsg:
      return severity::normal;
    case QtWarningMsg:
      return severity::warning;
    case QtCriticalMsg:
    case QtFatalMsg:
      break;
  }
  return severity::error;
}

void mold::log_handler(QtMsgType type, const QMessageLogContext&,
                       const QString& message) {
  if (g_gui_logger == nullptr) {
    return;
  }
  const auto severity = cast_qtmsgtype_to_severity(type);
  LOGGING_LOG(g_gui_logger->m_logger, severity) << message.toStdString();
}

using namespace mold;

gui_logger::gui_logger() : m_logger{"qt"} {
  g_gui_logger = this;
  qInstallMessageHandler(log_handler);
}

gui_logger::~gui_logger() {
  g_gui_logger = nullptr;
  qInstallMessageHandler(nullptr);
}
