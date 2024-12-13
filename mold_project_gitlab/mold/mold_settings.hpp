#ifndef MOLD_MOLD_SETTINGS_HPP
#define MOLD_MOLD_SETTINGS_HPP

#include "building_type.hpp"
#include "substrate_type.hpp"

namespace mold {

struct mold_settings {
  building_type building_type_;
  float frsi;
  substrate_type substrate_type_;

  bool operator==(const mold_settings& compare) const;
  bool operator!=(const mold_settings& compare) const;
};

std::ostream& operator<<(std::ostream& out, const mold_settings& to_print);
}  // namespace mold

#endif  // MOLD_MOLD_SETTINGS_HPP
