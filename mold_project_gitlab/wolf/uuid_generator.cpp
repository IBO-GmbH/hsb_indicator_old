#include "uuid_generator.hpp"
#include <boost/uuid/random_generator.hpp>
#include "base64.hpp"

using namespace wolf;

static boost::uuids::random_generator g_generator;

uuid_generator::uuid_array uuid_generator::generate_as_array() {
  const boost::uuids::uuid uuid(g_generator());
  uuid_generator::uuid_array result;
  std::copy(uuid.begin(), uuid.end(), result.begin());

  return result;
}

std::string uuid_generator::generate_as_base64_string() {
  return base64::encode(generate_as_array());
}
