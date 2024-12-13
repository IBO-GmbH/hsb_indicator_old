#ifndef MOLD_GUI_SENSORVALUETYPETOTEXT_HPP
#define MOLD_GUI_SENSORVALUETYPETOTEXT_HPP

#include <QObject>
#include "wolf/sensor_value_type.hpp"

namespace mold {

class SensorValueTypeToText : public QObject {
  Q_OBJECT
 public:
  static std::optional<QString> description(const wolf::sensor_value_type type);
  static std::optional<QString> unit(const wolf::sensor_value_type type);
};
}  // namespace mold
#endif  // SENSORVALUETYPETOTEXT_HPP
