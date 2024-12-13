#ifndef WOLF_ONBOARD_SENSOR_CHECKER_HPP
#define WOLF_ONBOARD_SENSOR_CHECKER_HPP

#include "wolf/room.hpp"

namespace wolf {

class onboard_sensor_checker {
 public:
  using sensors = std::vector<sensor_id>;

  static bool has_onboard_sensor(const room& room_);
  static bool has_sht25(const room& room_);
  static bool has_shtc1(const room& room_);
  static bool has_scd30(const room& room_);
  static bool has_sgpc3(const room& room_);
  static bool has_ee895(const room& room_);
  static bool has_sht85(const room& room_);
  static bool has_tmp117(const room& room_);

 private:
  static bool check_sensor(const room& room_, const sensors& to_check);
};
}  // namespace wolf

#endif  // WOLF_ONBOARD_SENSOR_CHECKER_HPP
