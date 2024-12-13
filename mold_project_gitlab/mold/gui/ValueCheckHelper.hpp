#ifndef MOLD_VALUECHECKHELPER_HPP
#define MOLD_VALUECHECKHELPER_HPP

#include <QLabel>
#include <QWidget>

#include "mold/configuration_values.hpp"

namespace mold {

class ValueCheckHelper : public QWidget {
  Q_OBJECT

 public:
  using timestamp = std::chrono::system_clock::time_point;
  using config_value = configuration_values::value;
  using optional_value = boost::optional<config_value>;
  using labels = std::vector<QLabel *>;
  using duration = std::chrono::system_clock::duration;

  static bool isValueValid(const optional_value &value, const timestamp &now,
                           const duration &time_reference);
  static bool checkIfOld(const timestamp &to_check, const timestamp &now,
                         const duration &time_reference);
  static void changeColourIfOld(const labels &to_change, const bool old);
  static void setNoData(QLabel &number, QLabel *decimal = nullptr);

 private:
  static bool checkTimeOld(const timestamp &now, const timestamp &value_time,
                           const duration &to_check);
  static void setGrey(QLabel *label);
  static void setDefault(QLabel *label);
  static void setLabel(QLabel *label, const QString &style);
};
}  // namespace mold

#endif  // MOLD_VALUECHECKHELPER_HPP
