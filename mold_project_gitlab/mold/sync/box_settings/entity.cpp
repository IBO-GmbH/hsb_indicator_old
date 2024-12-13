#include "entity.hpp"

std::ostream &mold::sync::box_settings::operator<<(std::ostream &out,
                                                   const entity &to_print) {
  return out << "{building_type_:" << to_print.building_type_
             << ", threshold_factor:" << to_print.threshold_factor
             << ", substrate_type:" << to_print.substrate_type_
             << ", outdoor_sensor:" << to_print.outdoor_sensor << "}";
}
