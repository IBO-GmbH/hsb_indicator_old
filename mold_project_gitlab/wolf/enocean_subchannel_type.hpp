#ifndef WOLF_ENOCEAN_SUBCHANNEL_TYPE_HPP
#define WOLF_ENOCEAN_SUBCHANNEL_TYPE_HPP

#include <ostream>

namespace wolf {

enum class enocean_subchannel_type {
  // DON'T change order. affects sensor_id serialisation.
  particles_pm10,
  particles_pm2p5,
  particles_pm1,
  current_channel1,
  current_channel2,
  current_channel3,
};

enum class types_with_subchannel {
  // DON'T change order. affects sensor_id serialisation.
  particles,
  current,
};

std::ostream& operator<<(std::ostream& out,
                         const enocean_subchannel_type to_print);
}  // namespace wolf

#endif  // WOLF_ENOCEAN_SUBCHANNEL_TYPE_HPP
