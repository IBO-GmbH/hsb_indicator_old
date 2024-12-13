#ifndef MOLD_BUILDING_TYPE_HPP
#define MOLD_BUILDING_TYPE_HPP

#include <ostream>

namespace mold {

// TODO removal of "none"?
enum class building_type : std::uint32_t { old, new_, none };
std::ostream& operator<<(std::ostream& out, const building_type type);
}  // namespace mold

#endif  // MOLD_BUILDING_TYPE_HPP
