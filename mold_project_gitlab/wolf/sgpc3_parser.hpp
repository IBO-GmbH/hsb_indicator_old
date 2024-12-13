#ifndef WOLF_SGPC3_PARSER_HPP
#define WOLF_SGPC3_PARSER_HPP

#include <array>
#include <boost/optional.hpp>
#include "log/logger.hpp"

namespace wolf {

class sgpc3_parser {
 public:
  sgpc3_parser();

  using buffer = std::array<std::uint8_t, 3>;
  boost::optional<float> handle_data(const buffer &buffer) const;

 private:
  mutable logging::logger m_logger;
};

}  // namespace wolf

#endif  // WOLF_SGPC3_PARSER_HPP
