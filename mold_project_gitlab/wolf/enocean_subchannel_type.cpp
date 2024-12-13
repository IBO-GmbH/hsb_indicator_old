#include "enocean_subchannel_type.hpp"
#include <boost/assert.hpp>

std::ostream &wolf::operator<<(std::ostream &out,
                               const wolf::enocean_subchannel_type to_print) {
  switch (to_print) {
    case enocean_subchannel_type::particles_pm10:
      return out << "particles_pm10";
    case enocean_subchannel_type::particles_pm2p5:
      return out << "particles_pm2p5";
    case enocean_subchannel_type::particles_pm1:
      return out << "particles_pm1";
    case enocean_subchannel_type::current_channel1:
      return out << "current_channel1";
    case enocean_subchannel_type::current_channel2:
      return out << "current_channel2";
    case enocean_subchannel_type::current_channel3:
      return out << "current_channel3";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}
