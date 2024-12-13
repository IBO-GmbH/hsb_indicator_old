#ifndef LATEST_VALUE_HELPER_HPP
#define LATEST_VALUE_HELPER_HPP

#include "mold/sprout/data_types.hpp"
#include "mold/sprout_value.hpp"

namespace mold::sprout {

struct latest_value_helper {
  const data_types::value& value;
  sprout_value operator()() const;
};

}  // namespace mold::sprout

#endif
