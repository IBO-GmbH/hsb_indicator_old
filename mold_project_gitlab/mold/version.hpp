#ifndef MOLD_VERSION_HPP
#define MOLD_VERSION_HPP

#include <string>

namespace mold {
class version {
 public:
  version() = delete;
  ~version() = delete;

  static std::string get_string();
  static std::string get_commit_hash();
  static std::string get_version_and_hash() {
    std::string result = get_string();
    result += "+";
    result += get_commit_hash();
    return result;
  }
};
}  // namespace mold

#endif  // MOLD_VERSION_HPP
