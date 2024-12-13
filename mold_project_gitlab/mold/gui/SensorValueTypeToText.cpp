#include "SensorValueTypeToText.hpp"

using namespace mold;

std::optional<QString> SensorValueTypeToText::description(
    const wolf::sensor_value_type type) {
  using namespace wolf;
  switch (type) {
    case sensor_value_type::temperature:
      return tr("Temperature");
    case sensor_value_type::humidity:
      return tr("Humidity");
    case sensor_value_type::co2:
      return "CO₂";
    case sensor_value_type::voc:
      return tr("Air quality");
    case sensor_value_type::sprout:
      return tr("Sprout");
    case sensor_value_type::particles_pm10:
      return "PM10";
    case sensor_value_type::particles_pm2p5:
      return "PM2.5";
    case sensor_value_type::particles_pm1:
      return "PM1";
    case sensor_value_type::evaluation_humidity:
      return tr("Evaluation humidity");
    case sensor_value_type::mold_state:
      return tr("Mold State");
    case sensor_value_type::frsi:
      return tr("frsi");
    case sensor_value_type::substrate:
      return tr("Substrate");
    case sensor_value_type::sensor_rssi:
      return "Sensor RSSI";
    case sensor_value_type::electric_current:
      return tr("Current");
    case sensor_value_type::electric_power:
      return tr("Power");
    case sensor_value_type::electric_energy:
    case wolf::sensor_value_type::electric_energy_difference:
      return tr("Energy");
    case wolf::sensor_value_type::air_pressure:
      return tr("Air Pressure");
    case wolf::sensor_value_type::light:
      return tr("Lighting");
    case wolf::sensor_value_type::motion:
      return tr("Motion");
    case wolf::sensor_value_type::acceleration:
      return tr("Acceleration");
    case wolf::sensor_value_type::supply_voltage:
      return tr("Supply Voltage");
    case wolf::sensor_value_type::presence:
      return tr("Presence");
    default:
      break;
  }
  return std::optional<QString>();
}

std::optional<QString> SensorValueTypeToText::unit(
    const wolf::sensor_value_type type) {
  using namespace wolf;
  switch (type) {
    case sensor_value_type::temperature:
      return "℃";
    case sensor_value_type::humidity:
      return "%";
    case sensor_value_type::co2:
      return "ppm";
    case sensor_value_type::voc:
      return "ppb";
    case sensor_value_type::particles_pm10:
    case sensor_value_type::particles_pm2p5:
    case sensor_value_type::particles_pm1:
      return "µg/m³";
    case sensor_value_type::evaluation_humidity:
      return "%";
    case sensor_value_type::electric_current:
      return "A";
    case sensor_value_type::electric_power:
      return "W";
    case sensor_value_type::electric_energy:
    case wolf::sensor_value_type::electric_energy_difference:
      return "kWh";
    case wolf::sensor_value_type::air_pressure:
      return "hPa";
    case wolf::sensor_value_type::light:
      return "lux";
    case wolf::sensor_value_type::presence:
    case wolf::sensor_value_type::motion:
      return "";
    case wolf::sensor_value_type::acceleration:
      return "m/s²";
    case wolf::sensor_value_type::supply_voltage:
      return "V";
    default:
      break;
  }
  return std::optional<QString>();
}
