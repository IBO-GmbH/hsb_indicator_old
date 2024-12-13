#include "sensor_metadata_creator.hpp"
#include "wolf/sensor_id_enocean.hpp"

wolf::sensor_metadata wolf::sensor_metadata_creator::enocean(
    const float min, const float max, const wolf::types::eep &eep,
    const wolf::types::id_esp3 &id) {
  return {"enocean",
          min,
          max,
          {{"eep", sensor_id_enocean::eep_string(eep)},
           {"sender_id", sensor_id_enocean::esp3_id_string(id)}}};
}

wolf::onboard_metadata wolf::sensor_metadata_creator::sht25(
    const std::optional<sensor_type> second_sensor) {
  return {{metadata_temperature(sensor_type::sht25)},
          {"sht25", 0.f, 100.f, {}},
          metadata_co2(second_sensor)};
}

wolf::onboard_metadata wolf::sensor_metadata_creator::shtc1(
    const std::optional<sensor_type> second_sensor) {
  return {{metadata_temperature(sensor_type::shtc1)},
          {"shtc1", 0.f, 100.f, {}},
          metadata_co2(second_sensor)};
}

wolf::onboard_metadata wolf::sensor_metadata_creator::scd30(
    const std::optional<sensor_type> second_sensor) {
  return {{metadata_temperature(sensor_type::scd30)},
          {"scd30", 0.f, 100.f, {}},
          metadata_co2(second_sensor)};
}

wolf::sensor_metadata wolf::sensor_metadata_creator::spgc3() {
  return {"sgpc3", 0.f, 60000.f, {}};
}

wolf::onboard_metadata wolf::sensor_metadata_creator::ee895(
    const std::optional<wolf::sensor_type> temperature_sensor) {
  return {{metadata_temperature(temperature_sensor)},
          {"ee895", 700.f, 1100.f, {}},
          metadata_co2(sensor_type::ee895)};
}

wolf::onboard_metadata wolf::sensor_metadata_creator::sht85() {
  return {{metadata_temperature(sensor_type::sht85)},
          {"sht85", 0.f, 100.f, {}},
          {}};
}

wolf::sensor_metadata wolf::sensor_metadata_creator::tmp117() {
  return {metadata_temperature(sensor_type::sht85)};
}

void wolf::sensor_metadata_creator::current_or_power(
    sensor_metadata &current_data, const wolf::types::id &sensor_id) {
  const auto last_byte = sensor_id.back();
  // is sum
  if (((last_byte + 1) % 4) == 0) current_data.max_value *= 3;
  // is power
  if (last_byte > 3 && last_byte < 8) current_data.max_value *= 230;
}

wolf::sensor_metadata wolf::sensor_metadata_creator::actor(
    const unsigned int actor_number, const wolf::types::id_esp3 &actor_id) {
  return {"enocean actor",
          0.f,
          1.f,
          {{"actor_number", std::to_string(actor_number)},
           {"actor_id", sensor_id_enocean::esp3_id_string(actor_id)}}};
}

wolf::sensor_metadata_creator::questionnaire_metadata
wolf::sensor_metadata_creator::questionnaire() {
  return {{"gender", 0.f, 1.f, {}},       {"age", 0.f, 4.f, {}},
          {"weight", 0.f, 4.f, {}},       {"height", 0.f, 4.f, {}},
          {"clothing", 0.f, 4.f, {}},     {"activity", 0.f, 4.f, {}},
          {"temperature", -2.f, 2.f, {}}, {"thermic comfort", -3.f, 3.f, {}},
          {"heating", -2.f, 2.f, {}},     {"air quality", -2.f, 2.f, {}},
          {"humidity", -2.f, 2.f, {}}};
}

wolf::sensor_metadata wolf::sensor_metadata_creator::metadata_co2(
    const std::optional<sensor_type> co2_sensor) {
  if (!co2_sensor.has_value()) return {};
  if (*co2_sensor == sensor_type::scd30) return {"scd30", 0.f, 40000.f, {}};
  if (*co2_sensor == sensor_type::ee895) return {"ee895", 0.f, 5000.f, {}};
  return {};
}

wolf::sensor_metadata wolf::sensor_metadata_creator::metadata_temperature(
    const std::optional<wolf::sensor_type> temperature_sensor) {
  if (temperature_sensor.has_value()) {
    if (*temperature_sensor == sensor_type::sht25)
      return {"sht25", -40.f, 125.f, {}};
    if (*temperature_sensor == sensor_type::shtc1)
      return {"shtc1", -30.f, 100.f, {}};
    if (*temperature_sensor == sensor_type::scd30)
      return {"scd30", -40.f, 70.f, {}};
    if (*temperature_sensor == sensor_type::sht85)
      return {"sht85", -40.f, 105.f, {}};
    if (*temperature_sensor == sensor_type::tmp117)
      return {"tmp", -55.f, 150.f, {}};
  }
  return {"ee895", -40.f, 60.f, {}};
}
