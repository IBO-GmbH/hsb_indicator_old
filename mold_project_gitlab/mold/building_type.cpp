#include "building_type.hpp"

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::building_type type) {
  switch (type) {
    case building_type::new_:
      out << "building_type::new";
      break;
    case building_type::old:
      out << "building_type::old";
      break;
    default:
      out << "building_type::none";
      break;
  }
  return out;
}
