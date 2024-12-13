#include "RoomNameHandler.hpp"

#include <QFontMetrics>
#include "wolf/room_name_index_handler.hpp"

using namespace logging;

mold::RoomNameHandler::RoomNameHandler(const DataExchange &dataExchange)
    : m_logger("RoomNameHandler"), m_dataExchange(dataExchange) {}

QStringList mold::RoomNameHandler::generateRoomList() const {
  QStringList roomNames = {tr("Living room"), tr("Nursery"), tr("Bedroom"),
                           tr("Kitchen"),     tr("Cellar"),  tr("Office"),
                           tr("Bathroom")};
  return roomNames;
}

int mold::RoomNameHandler::getNameIndex(const QStringList &room_list,
                                        const QString &room_name) const {
  if (room_name.isEmpty()) return 0;
  return room_list.indexOf(room_name, 0);
}

void mold::RoomNameHandler::addIfNewName(QStringList &room_list,
                                         const QString &room_name) const {
  if (room_list.contains(room_name)) return;
  room_list.prepend(room_name);
}

QStringList mold::RoomNameHandler::cropRoomNameList(
    const QStringList &room_list, const QFont &font, const int width) const {
  QStringList cropped_list;
  for (auto &room : room_list)
    cropped_list.append(getCroppedRoomName(font, width, room));
  return cropped_list;
}

std::string mold::RoomNameHandler::getDefaultRoomName() const {
  auto &rooms = m_dataExchange.get_rooms();
  auto index = wolf::room_name_index_handler::get_room_index(rooms);
  QString name(tr("Room "));
  name += QString::fromStdString(std::to_string(index));
  return name.toStdString();
}

QString mold::RoomNameHandler::getCroppedRoomName(const QFont &font,
                                                  const int width,
                                                  const QString &name) {
  QFontMetrics metrix(font);
  int new_width = width - 2;
  return metrix.elidedText(name, Qt::ElideRight, new_width);
}
