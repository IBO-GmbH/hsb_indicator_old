#include "url_helper.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <iomanip>
#include <sstream>

std::string wolf::url_helper::combine_url_with_device_id(
    const std::string &url, const uint64_t device_id) {
  auto result_url = url;
  const auto device_id_casted = cast_device_id_to_string(device_id);
  boost::replace_all(result_url, "<device_id>", device_id_casted);
  return result_url;
}

std::string wolf::url_helper::cast_device_id_to_string(
    const uint64_t device_id) {
  std::stringstream result;
  static constexpr auto six_bytes{6};
  result << std::setfill('0') << std::setw(six_bytes * 2) << std::hex
         << device_id;
  return result.str();
}
