#include "substrate_type.hpp"

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::substrate_type type) {
  switch (type) {
    case substrate_type::type_one:
      out << "substrate_type::type_one";
      break;
    case substrate_type::type_two:
      out << "substrate_type::type_two";
      break;
    default:
      out << "substrate_type::none";
      break;
  }
  return out;
}
