#ifndef WOLF_NETWORK_BOND_DESERIALIZER_HPP
#define WOLF_NETWORK_BOND_DESERIALIZER_HPP

#include <bond/core/bond.h>
#include "mold_protocol_reflection.h"

namespace wolf::network {

class bond_deserializer {
 public:
  bond_deserializer();

  template <class message_type>
  [[nodiscard]] message_type parse_message(
      const bond::bonded<Clima::MessageEntry>& entry) {
    const bond::bonded<message_type> data_bonded(entry);
    message_type response;
    data_bonded.Deserialize(response);
    return response;
  }
};

}  // namespace wolf::network
#endif
