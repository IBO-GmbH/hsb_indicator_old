#include "sgpc3_parser.hpp"
#include "wolf/sensirion/uint16_parser.hpp"
#include "wolf/std_array_printer.hpp"

using namespace logging;
using namespace wolf;

sgpc3_parser::sgpc3_parser() : m_logger{"sgpc3_parser"} {}

boost::optional<float> sgpc3_parser::handle_data(
    const sgpc3_parser::buffer &buffer) const {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_data, buffer:" << std_array_printer::to_string(buffer);

  const auto tvoc = sensirion::uint16_parser::check_crc8_and_cast(buffer);
  if (!tvoc.has_value()) {
    LOGGING_LOG(m_logger, severity::error) << "failed to parse tvoc";
    return {};
  }

  LOGGING_LOG(m_logger, severity::verbose)
      << "parsed data tvoc:" << std::dec << *tvoc;
  return static_cast<float>(*tvoc);
}
