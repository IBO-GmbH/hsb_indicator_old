#include "uuid_helper.hpp"
#include <boost/stacktrace/stacktrace.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

wolf::types::uuid_array wolf::uuid_helper::vector_to_array(
    const wolf::types::data &convert) const {
  types::uuid_array result;
  if (result.size() != convert.size()) {
    std::stringstream description;
    description << "uuid_helper::vector_to_array: size "
                   "of vector to convert does not match. convert:"
                << std::hex;
    std::copy(convert.cbegin(), convert.cend(),
              std::ostream_iterator<int>(description, " "));
    description << ", stacktrace:" << boost::stacktrace::stacktrace();
    throw std::runtime_error(description.str());
  }
  std::copy(convert.cbegin(), convert.cend(), result.begin());
  return result;
}

wolf::types::data wolf::uuid_helper::array_to_vector(
    const wolf::types::uuid_array &convert) const {
  const types::data result(convert.cbegin(), convert.cend());
  return result;
}

std::string wolf::uuid_helper::array_to_string(
    const wolf::types::uuid_array &to_cast) const {
  boost::uuids::uuid uuid;
  std::copy(to_cast.cbegin(), to_cast.cend(), uuid.begin());
  return boost::uuids::to_string(uuid);
}

wolf::types::uuid_array wolf::uuid_helper::string_to_array(
    const std::string &to_cast) const {
  const boost::uuids::string_generator generator;
  const boost::uuids::uuid uuid = generator(to_cast);
  wolf::types::uuid_array result;
  std::copy(uuid.begin(), uuid.end(), result.begin());
  return result;
}
