#include <QApplication>
#include <QFile>
#include <memory>
#include "gsm_confirmation_file.hpp"
#include "gui/ConfirmationForm.hpp"

int main(int argc, char* argv[]) {
  if (gsm_confirmation_file::read_confirmed()) return 0;

  QApplication app(argc, argv);

  QFile stylesheetFile("stylesheet.qss");
  if (!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw std::invalid_argument("failed to open stylesheet file");
  } else {
    const QByteArray content = stylesheetFile.readAll();
    app.setStyleSheet(QString::fromLatin1(content));
  }

  mold::ConfirmationForm window;
  window.show();

  return app.exec();
}
