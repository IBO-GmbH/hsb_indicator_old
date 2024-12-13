#include "mold_settings.hpp"
#include "wolf/math_compare_decimals.hpp"

bool mold::mold_settings::operator==(const mold::mold_settings &compare) const {
  return building_type_ == compare.building_type_ &&
         wolf::math::compare_floats(frsi,
                                    compare.frsi) &&
         substrate_type_ == compare.substrate_type_;
}

bool mold::mold_settings::operator!=(const mold::mold_settings &compare) const {
  return !(*this == compare);
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::mold_settings &to_print) {
  return out << "{building: " << to_print.building_type_
             << ", limit: " << to_print.frsi
             << ", substrate_type: " << to_print.substrate_type_ << "}";
}
