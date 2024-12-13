#ifndef BLUB_CORE_BASE64_HPP
#define BLUB_CORE_BASE64_HPP

#include <sstream>
#include <vector>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#define USE_BASE64_PADDING 0

namespace wolf {

// http://stackoverflow.com/questions/7053538/how-do-i-encode-a-string-to-base64-using-only-boost
class base64 {
 public:
  template <typename data_type>
  static std::string encode(const data_type &to_encode) {
    namespace bai = boost::archive::iterators;
#if USE_BASE64_PADDING
    static const std::array<std::string, 3> base64_padding{"", "==", "="};
#endif
    std::stringstream os;

    // convert binary values to base64 characters
    using base64_enc = bai::base64_from_binary<
        bai::transform_width<typename data_type::const_iterator, 6, 8> >;

    std::copy(base64_enc(to_encode.cbegin()), base64_enc(to_encode.cend()),
              std::ostream_iterator<std::uint8_t>(os));
#if USE_BASE64_PADDING
    os << base64_padding[to_encode.size() % 3];
#endif
    return os.str();
  }

  template <typename container_type = std::string,
            typename result_type = std::vector<std::uint8_t> >
  static result_type decode(const container_type &work) {
    namespace bai = boost::archive::iterators;

    using base64_dec = bai::transform_width<
        bai::binary_from_base64<typename container_type::const_iterator>, 8, 6>;

    std::size_t size = work.size();
#if USE_BASE64_PADDING
    // Remove the padding characters, cf.
    // https://svn.boost.org/trac/boost/ticket/5629
    if (size != 0 && work[size - 1] == '=') {
      --size;
      if (size != 0 && work[size - 1] == '=') {
        --size;
      }
    }
#endif
    if (size == 0) {
      return result_type();
    }

    result_type result;
    std::copy(base64_dec(work.cbegin()), base64_dec(work.cend()),
              std::back_inserter(result));
    return result;
  }

 private:
};

#undef USE_BASE64_PADDING
}

#endif  // BLUB_CORE_BASE64_HPP
