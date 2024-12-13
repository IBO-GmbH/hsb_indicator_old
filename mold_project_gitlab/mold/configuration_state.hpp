#ifndef MOLD_CONFIGURATION_STATE_HPP
#define MOLD_CONFIGURATION_STATE_HPP

#include <boost/optional.hpp>
#include <chrono>
#include "calculation_state.hpp"

namespace mold {

struct configuration_state {
  using time_point =
      boost::optional<std::chrono::time_point<std::chrono::system_clock>>;

  calculation_state state{calculation_state::green};
  time_point time_since_green;
  time_point time_since_not_green;  // TODO does not get used. remove?
  // time since last green value got calculated
  time_point time_since_first_green;

  bool operator==(const configuration_state& compare) const;
  bool operator!=(const configuration_state& compare) const;
};

std::ostream& operator<<(std::ostream& out,
                         const configuration_state& to_print);
}  // namespace mold

#endif  // MOLD_CONFIGURATION_STATE_HPP
