#include "enocean_channel_helper.hpp"
#include "log/logger.hpp"

wolf::sensor_value_type wolf::enocean_channel_helper::cast_to_wolf(
    const CHANNEL_TYPE type,
    const std::optional<enocean_subchannel_type> &subchannel,
    const types::eep &eep) {
  switch (type) {
    case S_TEMP:
      if (eep_is_heating_valve(eep))
        return sensor_value_type::valve_temperature;
      return sensor_value_type::temperature;
    case S_RELHUM:
      return sensor_value_type::humidity;
    case S_CONC:
      return sensor_value_type::co2;
    case E_VOC:
      return sensor_value_type::voc;
    case S_PERCENTAGE:
      return sensor_value_type::sprout;
    case S_CURRENT:
      return sensor_value_type::electric_current;
    case S_PRESSURE:
      return sensor_value_type::air_pressure;
    case S_PARTICLES:
      if (!subchannel.has_value()) break;
      switch (subchannel.value()) {
        case wolf::enocean_subchannel_type::particles_pm10:
          return sensor_value_type::particles_pm10;
        case wolf::enocean_subchannel_type::particles_pm2p5:
          return sensor_value_type::particles_pm2p5;
        case wolf::enocean_subchannel_type::particles_pm1:
          return sensor_value_type::particles_pm1;
        default:
          break;
      }
      break;
    case S_LUMINANCE:
      return sensor_value_type::light;
    case E_WINDOWHANDLE:
      return sensor_value_type::magnet_switch;
    case F_OCCUPIED:
      if (eep_is_presence(eep)) return sensor_value_type::presence;
      return sensor_value_type::motion;
    case S_ACCEL:
      return sensor_value_type::acceleration;
    case S_VOLTAGE:
      return sensor_value_type::supply_voltage;
    case S_SETPOINT:
      if (eep_is_heating_valve(eep)) return sensor_value_type::valve_position;
      break;
    default:
      break;
  }
  logging::logger logger{"wolf::enocean_channel_helper"};
  LOGGING_LOG(logger, logging::severity::warning)
      << "cast to wolf: could not cast " << std::dec << static_cast<int>(type);
  return sensor_value_type::unknown;
}

std::optional<wolf::types_with_subchannel>
wolf::enocean_channel_helper::get_type_with_subchannel(
    const CHANNEL_TYPE type) {
  switch (type) {
    case S_CURRENT:
      return types_with_subchannel::current;
    case S_PARTICLES:
      return types_with_subchannel::particles;
    default:
      return std::optional<types_with_subchannel>();
  }
}

bool wolf::enocean_channel_helper::eep_is_presence(
    const wolf::types::eep &eep) {
  return eep[0] == 0xa5 && eep[1] == 0x07 && eep[2] == 0x01;
}

bool wolf::enocean_channel_helper::eep_is_heating_valve(
    const wolf::types::eep &eep) {
  return eep[0] == 0xa5 && eep[1] == 0x20 && eep[2] == 0x01;
}
