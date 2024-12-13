#ifndef WOLF_URL_HELPER_HPP
#define WOLF_URL_HELPER_HPP

#include <string>

namespace wolf {

class url_helper {
 public:
  static std::string combine_url_with_device_id(const std::string& url,
                                                const std::uint64_t device_id);

 private:
  static std::string cast_device_id_to_string(const std::uint64_t device_id);
};
}  // namespace wolf

#endif  // WOLF_URL_HELPER_HPP
