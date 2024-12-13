#ifndef WOLF_ACTOR_ACTOR_HPP
#define WOLF_ACTOR_ACTOR_HPP

#include "wolf/types.hpp"

namespace wolf {

namespace actor {

struct actor {
  types::id_esp3 id;
  bool state;
};
bool operator==(const actor& first, const actor& second);
bool operator!=(const actor& first, const actor& second);

std::ostream& operator<<(std::ostream& out, const actor& to_print);
}
}

#endif  // WOLF_ACTOR_ACTOR_HPP
