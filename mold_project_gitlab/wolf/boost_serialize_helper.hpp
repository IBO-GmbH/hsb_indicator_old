#ifndef WOLF_BOOST_SERIALIZE_HELPER_HPP
#define WOLF_BOOST_SERIALIZE_HELPER_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/array.hpp>
#include "base64.hpp"

namespace wolf {
namespace serialize {

template <class conversion_type>
std::string encode_to_base64(const conversion_type &to_encode) {
  std::ostringstream result;
  {
    boost::archive::binary_oarchive oarchive(result);
    oarchive << to_encode;
  }
  result.flush();
  return base64::encode(result.str());
}

template <class conversion_type>
void decode_from_base64(const std::string &encoded, conversion_type &result) {
  const auto input_decoded = base64::decode(encoded);
  const std::string input_casted(
      reinterpret_cast<const char *>(input_decoded.data()),
      input_decoded.size());
  std::istringstream input(input_casted);
  boost::archive::binary_iarchive iarchive(input);
  iarchive >> result;
}
}  // namespace serialize
}  // namespace wolf

#endif  // WOLF_BOOST_SERIALIZE_HELPER_HPP
