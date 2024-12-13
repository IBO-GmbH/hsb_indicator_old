#ifndef MOLD_SUBSTRATE_TYPE_HPP
#define MOLD_SUBSTRATE_TYPE_HPP

#include <ostream>

namespace mold {

enum class substrate_type : std::uint32_t { none, type_one, type_two };
std::ostream& operator<<(std::ostream& out, const substrate_type type);
}  // namespace mold

#endif  // MOLD_SUBSTRATE_TYPE_HPP
