#ifndef CONTROLLER_PER_ROOM_HPP
#define CONTROLLER_PER_ROOM_HPP

#include "controller.hpp"
#include "limits.hpp"
#include "svas1_ventilation.hpp"
#include "svas2_dehumidifier.hpp"
#include "svas3_heating.hpp"
#include "svas4_air_quality.hpp"

namespace wolf {

namespace controller {

struct controller_per_room {
  types::uuid_array room_id;
  limits limits_;
  std::shared_ptr<controller> controller_;
  std::shared_ptr<controller_ventilation> ventilation;
  std::shared_ptr<svas1_ventilation> svas1;
  std::shared_ptr<svas2_dehumidifier> svas2;
  std::shared_ptr<svas3_heating> svas3;
  std::shared_ptr<svas4_air_quality> svas4;
};
}
}

#endif  // CONTROLLER_PER_ROOM_HPP
