#ifndef WOLF_GZIP_HPP
#define WOLF_GZIP_HPP

#include <cstdint>
#include <vector>

namespace wolf {

class gzip {
 public:
  gzip() = delete;

  using data = std::vector<std::uint8_t>;
  static data compress(const data& to_compress);
  static data decompress(const data& to_decompress);
};
}

#endif  // WOLF_GZIP_HPP
