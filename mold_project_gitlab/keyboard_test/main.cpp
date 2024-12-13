#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QQuickItem>
#include <QQuickView>
#include <QQuickWindow>
#include "log/log.hpp"
#include "log/logger.hpp"

int main(int argc, char *argv[]) {
  logging::initialize();
  logging::logger logger{"main"};
  using namespace logging;

  LOGGING_LOG(logger, severity::verbose) << "logging ftw";
  qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

  QApplication application(argc, argv);
  QMainWindow window;
  window.show();

  LOGGING_LOG(logger, severity::normal) << "before quick";

  QQuickView *view = new QQuickView();
  QWidget *container = QWidget::createWindowContainer(view, &window);
  view->setSource(QUrl("qrc:/main.qml"));

  view->setResizeMode(QQuickView::SizeRootObjectToView);
  if (view->status() == QQuickView::Error)
    throw std::runtime_error("could not load main.qml");

  LOGGING_LOG(logger, severity::normal) << "after quick";

  window.setStyleSheet("background-color:blue;");
  window.setCentralWidget(container);

  return application.exec();
}
