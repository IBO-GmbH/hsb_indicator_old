#ifndef MOLD_INITIAL_ROOM_CREATOR_HPP
#define MOLD_INITIAL_ROOM_CREATOR_HPP

#include "mold/configuration_handler.hpp"
#include "mold/program_options_parser.hpp"
#include "mold/room_creator.hpp"
#include "sht25_handler.hpp"
#include "wolf/ee895_handler.hpp"
#include "wolf/enocean_handler.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/scd30_handler.hpp"
#include "wolf/sensor_metadata_creator.hpp"
#include "wolf/sgpc3_handler.hpp"
#include "wolf/sht85_handler.hpp"
#include "wolf/shtc1_handler.hpp"
#include "wolf/tmp117_handler.hpp"
#include "wolf/uuid_helper.hpp"

namespace mold {

// TODO refactor to two parts. To "initial_room_creator" and
// "initial_outdoor_sensor_creator"
class initial_room_creator {
 public:
  using sensors = std::vector<wolf::sensor_id>;
  using rooms = std::vector<wolf::room>;
  using optional_room = boost::optional<wolf::room>;

  struct handlers {
    wolf::scd30_handler &scd30_handler_;
    wolf::sgpc3_handler &sgpc3_handler_;
    sht25_handler &sht25_handler_;
    wolf::shtc1_handler &shtc1_handler_;
    wolf::ee895_handler &ee895_handler_;
    wolf::sht85_handler &sht85_handler_;
    wolf::tmp117_handler &tmp117_handler_;
  };

  initial_room_creator(const mold::config &program_options_,
                       wolf::profile_handler &profile_handler_,
                       room_creator &room_creator_,
                       configuration_handler &configuration_handler_,
                       wolf::enocean_handler &enocean_handler);
  optional_room create_initial_room(const bool outdoor_is_set);
  optional_room change_initial_room(const rooms &rooms_,
                                    const bool outdoor_is_set);
  void add_scd30(const std::optional<wolf::sensor_type> &second_sensor);
  void add_sgpc3();
  void add_sht25(const std::optional<wolf::sensor_type> &second_sensor);
  void add_shtc1(const std::optional<wolf::sensor_type> &second_sensor);
  void add_ee895(const std::optional<wolf::sensor_type> &second_sensor);
  void add_sht85(const std::optional<wolf::sensor_type> &second_sensor);
  void add_tmp117(const std::optional<wolf::sensor_type> &);
  void create_no_room();
  void resend_all_signals(const wolf::room &room, handlers &handlers_);

 private:
  void add_initial_outdoor_sensor();
  void add_outdoor_sensor(const wolf::types::id_esp3 sensor,
                          const wolf::types::eep &eep);
  optional_room get_initial_room(const rooms &rooms_) const;
  std::optional<wolf::sensor_id> get_temperature_sensor(
      const std::optional<wolf::sensor_type> &second_sensor) const;
  wolf::sensor_id get_humidity_sensor(
      const std::optional<wolf::sensor_type> &second_sensor) const;
  std::optional<wolf::sensor_id> get_co2_sensor(
      const std::optional<wolf::sensor_type> &second_sensor) const;
  void add_scd30_and_ee895_temperature_sensor(
      const std::optional<wolf::sensor_type> &second_sensor,
      const wolf::sensor_id &standard_id_temperature,
      const wolf::onboard_metadata &metadata);
  void add_sht25_and_shtc1_co2_sensor(
      const std::optional<wolf::sensor_type> &second_sensor,
      const wolf::onboard_metadata &metadata);
  void add_sensor_to_room_if_not_already_added(const wolf::sensor_id &to_add);
  void handle_sht25_and_shtc1_sensors(
      const wolf::sensor_type &sensor,
      const std::optional<wolf::sensor_type> &second_sensor);

 private:
  logging::logger m_logger;
  room_creator &m_creator;
  const mold::config m_program_options;
  wolf::profile_handler &m_profile_handler;
  configuration_handler &m_configuration_handler;
  wolf::enocean_handler &m_enocean_handler;

  sensors m_room_sensors;
  wolf::sensor_id m_indoor_temperature;
  wolf::sensor_id m_indoor_humidity;

  wolf::uuid_helper m_helper;
  const wolf::types::uuid_array m_initial_id;
};
}  // namespace mold

#endif  // MOLD_INITIAL_ROOM_CREATOR_HPP
