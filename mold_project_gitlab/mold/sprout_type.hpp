#ifndef MOLD_SPROUT_TYPE_HPP
#define MOLD_SPROUT_TYPE_HPP

#include <ostream>

namespace mold {

// TODO move to sprout::
enum struct sprout_type {
  // DON'T change order. affects sprout_type serialisation.
  infinite_days = 0,
  sixteen_days = 1,
  eight_days = 2,
  four_days = 3,
  two_days = 4,
  one_day = 5
};

std::ostream& operator<<(std::ostream& os, const sprout_type& out);
}  // namespace mold
#endif
