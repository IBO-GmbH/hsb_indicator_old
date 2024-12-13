#ifndef WOLF_BOOST_SERIALIZE_CHRONO_SYSTEM_CLOCK_TIMEPOINT_HPP
#define WOLF_BOOST_SERIALIZE_CHRONO_SYSTEM_CLOCK_TIMEPOINT_HPP

// http://lists.boost.org/boost-users/att-82112/chrono.hpp
// TODO move to namespace wolf

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/*////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8*/
// serialization/chrono.hpp:
// serialization for std::chrono templates

#include <boost/config.hpp>
#include <chrono>

#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {
// serialization for std::chrono::system_clock::time_point
template <class Archive>
void serialize(Archive& archive,
               std::chrono::system_clock::time_point& to_serialize,
               const unsigned int version) {
  boost::serialization::split_free(archive, to_serialize, version);
}

template <class Archive>
void save(Archive& archive,
          std::chrono::system_clock::time_point const& to_save,
          const unsigned int /* version */) {
  namespace chrono = std::chrono;
  chrono::milliseconds::rep millis =
      chrono::duration_cast<chrono::milliseconds>(to_save.time_since_epoch())
          .count();
  archive& millis;
}

template <class Archive>
void load(Archive& archive, std::chrono::system_clock::time_point& to_load,
          const unsigned int /* version */) {
  namespace chrono = std::chrono;
  chrono::milliseconds::rep millis;
  archive& millis;
  to_load = std::chrono::system_clock::time_point(chrono::milliseconds(millis));
}

}  // serialization
}  // namespace boost

#endif  // WOLF_BOOST_SERIALIZE_CHRONO_SYSTEM_CLOCK_TIMEPOINT_HPP
