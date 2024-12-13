#include "sync_state.hpp"

#include <cassert>

std::ostream &wolf::operator<<(std::ostream &out,
                               const wolf::sync_state &print) {
  switch (print) {
    case sync_state::synced:
      return out << "synced";
    case sync_state::insert:
      return out << "insert";
    case sync_state::update:
      return out << "update";
    case sync_state::remove:
      return out << "remove";
    default:
      assert(false);
      return out << "unknown";
  }
}
