#ifndef WOLF_TYPES
#define WOLF_TYPES

#include <array>
#include <vector>

namespace wolf::types {

// TODO refactor to boost::uuids::uuid
using uuid_array = std::array<std::uint8_t, 16>;
using id_esp3_array = std::array<std::uint8_t, 4>;
using id_esp3 = std::uint32_t;  // TODO rm me (not sure if make sense)
using eep = std::array<std::uint8_t, 3>;
using id = std::vector<std::uint8_t>;
using data = std::vector<std::uint8_t>;
}  // namespace wolf::types

namespace std {

::std::ostream &operator<<(::std::ostream &os,
                           const ::wolf::types::data &write);
::std::ostream &operator<<(::std::ostream &os,
                           const ::wolf::types::uuid_array &write);
::std::ostream &operator<<(::std::ostream &os,
                           const ::wolf::types::id_esp3_array &write);
::std::ostream &operator<<(::std::ostream &os, const ::wolf::types::eep &write);

template <typename T, size_t N>
struct hash<array<T, N> > {
  using argument_type = array<T, N>;
  using result_type = size_t;

  result_type operator()(const argument_type &a) const {
    hash<T> hasher;
    result_type h = 0;
    for (result_type i = 0; i < N; ++i) {
      h = h * 31 + hasher(a[i]);
    }
    return h;
  }
};
}  // namespace std

#endif  // WOLF_TYPES
