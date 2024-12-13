#ifndef MOLD_GUI_ROOMNAMEHANDLER_HPP
#define MOLD_GUI_ROOMNAMEHANDLER_HPP

#include <QObject>

#include "DataExchange.hpp"

namespace mold {

class RoomNameHandler : public QObject {
  Q_OBJECT
 public:
  explicit RoomNameHandler(const DataExchange& dataExchange);

  QStringList generateRoomList() const;
  void addIfNewName(QStringList& room_list, const QString& room_name) const;
  QStringList cropRoomNameList(const QStringList& room_list, const QFont& font,
                               const int width) const;
  int getNameIndex(const QStringList& room_list,
                   const QString& room_name) const;
  std::string getDefaultRoomName() const;
  static QString getCroppedRoomName(const QFont& font, const int width,
                                    const QString& name);

 private:
 private:
  mutable logging::logger m_logger;
  const std::vector<std::string> m_default_room_names;
  const DataExchange& m_dataExchange;
};
}  // namespace mold

#endif  // MOLD_GUI_ROOMNAMEHANDLER_HPP
