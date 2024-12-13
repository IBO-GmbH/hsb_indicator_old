#ifndef BOOST_SERIALIZE_CHRONO_SYSTEM_CLOCK_DURATION_HPP
#define BOOST_SERIALIZE_CHRONO_SYSTEM_CLOCK_DURATION_HPP

#include <boost/config.hpp>
#include <chrono>

#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {
// serialization for std::chrono::system_clock::duration
template <class Archive>
void serialize(Archive& archive,
               std::chrono::system_clock::duration& to_serialize,
               const unsigned int version) {
  boost::serialization::split_free(archive, to_serialize, version);
}

template <class Archive>
void save(Archive& archive, std::chrono::system_clock::duration const& to_save,
          const unsigned int /* version */) {
  namespace chrono = std::chrono;
  chrono::nanoseconds::rep nanoseconds =
      chrono::duration_cast<chrono::nanoseconds>(to_save).count();
  archive& nanoseconds;
}

template <class Archive>
void load(Archive& archive, std::chrono::system_clock::duration& to_load,
          const unsigned int /* version */) {
  namespace chrono = std::chrono;
  chrono::nanoseconds::rep nanoseconds;
  archive& nanoseconds;
  to_load =
      std::chrono::system_clock::duration(chrono::nanoseconds(nanoseconds));
}

}  // serialization
}  // namespace boost

#endif  // BOOST_SERIALIZE_CHRONO_SYSTEM_CLOCK_DURATION_HPP
