#include "sensor_value_type_helper.hpp"
#include <boost/stacktrace.hpp>
#include "log/logger.hpp"

using namespace wolf::network;
using namespace wolf;

Clima::Enums::SensorType sensor_value_type_helper::cast_to_network(
    const sensor_id& to_cast) {
  switch (to_cast.get_type()) {
    case sensor_value_type::temperature:
      return Clima::Enums::SensorType::Temperature_Celsius;
    case sensor_value_type::humidity:
      return Clima::Enums::SensorType::Humidity_Percent;
    case sensor_value_type::co2:
      return Clima::Enums::SensorType::CO2_PPM;
    case sensor_value_type::voc:
      return Clima::Enums::SensorType::TVOC_PPB;
    case sensor_value_type::particles_pm10:
      return Clima::Enums::SensorType::ParticlesPM10_ugM3;
    case sensor_value_type::particles_pm2p5:
      return Clima::Enums::SensorType::ParticlesPM2p5_ugM3;
    case sensor_value_type::particles_pm1:
      return Clima::Enums::SensorType::ParticlesPM1_ugM3;
    case sensor_value_type::evaluation_humidity:
      return Clima::Enums::SensorType::EvaluationHumidityRate_Percent;
    case sensor_value_type::mold_state:
      return Clima::Enums::SensorType::MoldState;
    case sensor_value_type::frsi:
      return Clima::Enums::SensorType::UsedFRSI;
    case sensor_value_type::substrate:
      return Clima::Enums::SensorType::UsedSubstrateType;
    case sensor_value_type::sensor_rssi:
      return Clima::Enums::SensorType::SensorSignalStrength_RSSI;
    case sensor_value_type::sprout:
      return Clima::Enums::SensorType::MoldSproutValue_Percent;
    case sensor_value_type::electric_current:
      switch (to_cast.get_id().back()) {
        case 0:
          return Clima::Enums::SensorType::CurrentPhase1_Amp;
        case 1:
          return Clima::Enums::SensorType::CurrentPhase2_Amp;
        case 2:
          return Clima::Enums::SensorType::CurrentPhase3_Amp;
        case 3:
          return Clima::Enums::SensorType::CurrentSum_Amp;
        default:
          break;
      }
      break;
    case sensor_value_type::electric_power:
      switch (to_cast.get_id().back()) {
        case 4:
          return Clima::Enums::SensorType::ElectricalPowerPhase1_Watt;
        case 5:
          return Clima::Enums::SensorType::ElectricalPowerPhase2_Watt;
        case 6:
          return Clima::Enums::SensorType::ElectricalPowerPhase3_Watt;
        case 7:
          return Clima::Enums::SensorType::ElectricalPowerSum_Watt;
        default:
          break;
      }
      break;
    case sensor_value_type::electric_energy:
      [[fallthrough]];
    case sensor_value_type::electric_energy_difference:
      return Clima::Enums::SensorType::ElectricalEnergy_kiloWattHours;
    case sensor_value_type::air_pressure:
      return Clima::Enums::SensorType::Barometric_hPa;
    case sensor_value_type::actor_switch:
    case sensor_value_type::magnet_switch:
    case sensor_value_type::motion:
    case sensor_value_type::presence:
      return Clima::Enums::SensorType::Switch_OnOff;
    case sensor_value_type::light:
      return Clima::Enums::SensorType::Light_lux;
      // just for testing, needs to be corrected if value should be sent
    case sensor_value_type::acceleration:
      return Clima::Enums::SensorType::WindSpeed_ms;
    case sensor_value_type::supply_voltage:
      return Clima::Enums::SensorType::Voltage_Volts;
    case sensor_value_type::questionnaire_gender:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerGender;
    case sensor_value_type::questionnaire_age:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerAge;
    case sensor_value_type::questionnaire_weight:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerWeight;
    case sensor_value_type::questionnaire_height:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerHeight;
    case sensor_value_type::questionnaire_clothing:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerClothing;
    case sensor_value_type::questionnaire_temperature:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerTemperature;
    case sensor_value_type::questionnaire_air_quality:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerAirQuality;
    case sensor_value_type::questionnaire_humidity:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerHumidity;
    case sensor_value_type::questionnaire_activity:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerActivity;
    case sensor_value_type::questionnaire_thermic_comfort:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerThermicComfort;
    case sensor_value_type::questionnaire_heating:
      return Clima::Enums::SensorType::ComfortQuestionnaireAnswerHeating;
    case sensor_value_type::valve_temperature:
      return Clima::Enums::SensorType::HeatingValveTemperature;
    case sensor_value_type::valve_position:
      return Clima::Enums::SensorType::HeatingValvePosition;
    case sensor_value_type::unknown:
      break;
  }
  logging::logger logger{"network::sensor_value_type_helper"};
  LOGGING_LOG(logger, logging::severity::error)
      << "cast_to_network: could not cast " << std::dec
      << static_cast<int>(to_cast.get_type())
      << ", stacktrace:" << boost::stacktrace::stacktrace();
  assert(false);
  return Clima::Enums::SensorType::None;
}

sensor_value_type sensor_value_type_helper::cast_to_wolf(
    const Clima::Enums::SensorType to_cast) {
  switch (to_cast) {
    case Clima::Enums::SensorType::Temperature_Celsius:
      return sensor_value_type::temperature;
    case Clima::Enums::SensorType::Humidity_Percent:
      return sensor_value_type::humidity;
    case Clima::Enums::SensorType::CO2_PPM:
      return sensor_value_type::co2;
    case Clima::Enums::SensorType::TVOC_PPB:
      return sensor_value_type::voc;
    case Clima::Enums::SensorType::ParticlesPM10_ugM3:
      return sensor_value_type::particles_pm10;
    case Clima::Enums::SensorType::ParticlesPM2p5_ugM3:
      return sensor_value_type::particles_pm2p5;
    case Clima::Enums::SensorType::ParticlesPM1_ugM3:
      return sensor_value_type::particles_pm1;
    case Clima::Enums::SensorType::EvaluationHumidityRate_Percent:
      return sensor_value_type::evaluation_humidity;
    case Clima::Enums::SensorType::MoldState:
      return sensor_value_type::mold_state;
    case Clima::Enums::SensorType::UsedFRSI:
      return sensor_value_type::frsi;
    case Clima::Enums::SensorType::SensorSignalStrength_RSSI:
      return sensor_value_type::sensor_rssi;
    case Clima::Enums::SensorType::MoldSproutValue_Percent:
      return sensor_value_type::sprout;
    case Clima::Enums::SensorType::CurrentPhase1_Amp:
    case Clima::Enums::SensorType::CurrentPhase2_Amp:
    case Clima::Enums::SensorType::CurrentPhase3_Amp:
    case Clima::Enums::SensorType::CurrentSum_Amp:
      return sensor_value_type::electric_current;
    case Clima::Enums::SensorType::ElectricalPowerPhase1_Watt:
    case Clima::Enums::SensorType::ElectricalPowerPhase2_Watt:
    case Clima::Enums::SensorType::ElectricalPowerPhase3_Watt:
    case Clima::Enums::SensorType::ElectricalPowerSum_Watt:
      return sensor_value_type::electric_power;
    case Clima::Enums::SensorType::ElectricalEnergy_kiloWattHours:
      return sensor_value_type::electric_energy_difference;
    case Clima::Enums::SensorType::Barometric_hPa:
      return sensor_value_type::air_pressure;
    case Clima::Enums::SensorType::Switch_OnOff:
      return sensor_value_type::actor_switch;
    case Clima::Enums::SensorType::Light_lux:
      return sensor_value_type::light;
    case Clima::Enums::SensorType::Voltage_Volts:
      return sensor_value_type::supply_voltage;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerGender:
      return sensor_value_type::questionnaire_gender;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerAge:
      return sensor_value_type::questionnaire_age;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerWeight:
      return sensor_value_type::questionnaire_weight;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerHeight:
      return sensor_value_type::questionnaire_height;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerClothing:
      return sensor_value_type::questionnaire_clothing;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerTemperature:
      return sensor_value_type::questionnaire_temperature;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerAirQuality:
      return sensor_value_type::questionnaire_air_quality;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerHumidity:
      return sensor_value_type::questionnaire_humidity;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerActivity:
      return sensor_value_type::questionnaire_activity;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerThermicComfort:
      return sensor_value_type::questionnaire_thermic_comfort;
    case Clima::Enums::SensorType::ComfortQuestionnaireAnswerHeating:
      return sensor_value_type::questionnaire_heating;
    case Clima::Enums::SensorType::HeatingValveTemperature:
      return sensor_value_type::valve_temperature;
    case Clima::Enums::SensorType::HeatingValvePosition:
      return sensor_value_type::valve_position;
    default:
      break;
  }
  logging::logger logger{"network::sensor_value_type_helper"};
  LOGGING_LOG(logger, logging::severity::error)
      << "cast_to_wolf: could not cast " << std::dec
      << static_cast<int>(to_cast)
      << ", stacktrace:" << boost::stacktrace::stacktrace();
  assert(false);
  return sensor_value_type::unknown;
}
