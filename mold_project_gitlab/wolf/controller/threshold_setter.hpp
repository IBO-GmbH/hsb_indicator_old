#ifndef WOLF_CONTROLLER_THRESHOLD_SETTER_HPP
#define WOLF_CONTROLLER_THRESHOLD_SETTER_HPP

#include "controller_per_room.hpp"

#include <vector>

namespace wolf {

namespace controller {

class threshold_setter {
 public:
  using controller_per_room_list = std::vector<controller_per_room>;
  using wait_time = std::chrono::system_clock::duration;

  threshold_setter(const wolf::config& program_options);
  void set_absolute_humidity(const controller_per_room_list::iterator& found,
                             const thresholds& absolute_humidity);
  void set_relative_humidity(const controller_per_room_list::iterator& found,
                             const limits::relative_humidity& limit_values);
  void set_thresholds_ventilation(
      const controller_per_room_list::iterator& found,
      const limits::relative_humidity& relative_humidity,
      const thresholds& absolute_humidity);
  void set_humidity_border(const std::shared_ptr<svas2_dehumidifier> svas2,
                           const float humidity_border, const float difference);
  void set_thresholds_temperature(const std::shared_ptr<svas3_heating> svas3,
                                  const limits::temperature& limit_values);
  void set_thresholds_air_quality(
      const controller_per_room_list::iterator& found,
      const limits::air_quality& limit_values);
  void set_co2(const controller_per_room_list::iterator& found,
               const limits::air_quality& limit_values);
  void set_sprout_value(const controller_per_room_list::iterator& found,
                        const limits::air_quality& limit_values);
  void set_voc(const controller_per_room_list::iterator& found,
               const limits::air_quality& limit_values);
  void set_all(const controller_per_room_list::iterator& found,
               const limits& limits, const thresholds& absolute_humidity);

 private:
  const config m_program_options;
};
}  // namespace controller
}  // namespace wolf

#endif  // WOLF_CONTROLLER_THRESHOLD_SETTER_HPP
