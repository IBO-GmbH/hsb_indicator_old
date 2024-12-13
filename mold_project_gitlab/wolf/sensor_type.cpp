#include "sensor_type.hpp"
#include "log/logger.hpp"

#include <cassert>

std::ostream& wolf::operator<<(std::ostream& os, const wolf::sensor_type out) {
  switch (out) {
    case wolf::sensor_type::none:
      return os << "none";
    case wolf::sensor_type::enocean:
      return os << "enocean";
    case wolf::sensor_type::sht25:
      return os << "sht25";
    case wolf::sensor_type::shtc1:
      return os << "shtc1";
    case wolf::sensor_type::sprout_value:
      return os << "sprout_value";
    case wolf::sensor_type::sgpc3:
      return os << "sgpc3";
    case wolf::sensor_type::scd30:
      return os << "scd30";
    case wolf::sensor_type::open_weather:
      return os << "open_weather";
    case wolf::sensor_type::evaluation_humidity:
      return os << "evaluation_humidity";
    case wolf::sensor_type::foreign:
      return os << "foreign";
    case wolf::sensor_type::mold_state:
      return os << "mold_state";
    case wolf::sensor_type::frsi:
      return os << "frsi";
    case wolf::sensor_type::substrate:
      return os << "substrate";
    case wolf::sensor_type::sensor_rssi:
      return os << "sensor_rssi";
    case wolf::sensor_type::actor_switch:
      return os << "actor_switch";
    case wolf::sensor_type::ee895:
      return os << "ee895";
    case wolf::sensor_type::comfort_questionnaire:
      return os << "comfort_questionnaire";
    case wolf::sensor_type::mqtt:
      return os << "mqtt";
    case wolf::sensor_type::sht85:
      return os << "sht85";
    case wolf::sensor_type::tmp117:
      return os << "tmp117";
  }

  logging::logger logger(
      "operator<<(std::ostream &os, const wolf::sensor_type &out)");

  LOGGING_LOG(logger, logging::severity::warning)
      << "sensor_type:" << static_cast<int>(out) << " is not handled";

  assert(false);
  return os << "undefined";
}

std::ostream& wolf::operator<<(std::ostream& out,
                               const wolf::sensor_sht25_channel to_print) {
  switch (to_print) {
    case sensor_sht25_channel::humidity:
      return out << "humidity";
    case sensor_sht25_channel::temperature:
      return out << "temperature";
  }
  assert(false);
  return out << "undefined";
}
