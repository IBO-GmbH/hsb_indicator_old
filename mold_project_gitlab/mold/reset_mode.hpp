#ifndef MOLD_RESET_MODE_HPP
#define MOLD_RESET_MODE_HPP

#include <ostream>

namespace mold {
enum struct reset_mode { shutdown, reboot };

std::ostream& operator<<(std::ostream& os, const reset_mode& out);
}  // namespace mold

#endif  // MOLD_RESET_MODE_HPP
