#ifndef WOLF_SYNC_STATE_HPP
#define WOLF_SYNC_STATE_HPP

#include <ostream>

namespace wolf {

// TODO delete?
enum struct sync_state : int {
  synced = 0,
  insert = 1,
  update = 2,
  remove = 3,
  invalid = 4
};

std::ostream& operator<<(std::ostream& out, const sync_state& print);
}  // namespace wolf

#endif  // WOLF_SYNC_STATE_HPP
