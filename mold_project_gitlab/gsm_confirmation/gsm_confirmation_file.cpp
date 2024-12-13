#include "gsm_confirmation_file.hpp"
#include <QFile>
#include <QString>

static const QString filename = "gsm_confirmed";

void gsm_confirmation_file::write_confirmed() {
  QFile file(filename);
  if (!file.open(QIODevice::OpenModeFlag::WriteOnly))
    throw std::runtime_error("write_confirmed() failed to open file");

  file.close();
}

bool gsm_confirmation_file::read_confirmed() { return QFile::exists(filename); }
