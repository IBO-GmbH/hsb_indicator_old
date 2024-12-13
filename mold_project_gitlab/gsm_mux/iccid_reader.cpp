#include "iccid_reader.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>

using namespace logging;

iccid_reader::iccid_reader(command_handler &command_)
    : m_logger{"iccid_reader"}, command(command_) {}

void iccid_reader::read(
    boost::interprocess::managed_shared_memory &shared_memory) {
  const std::string answer = command.write_at_command_and_verify_response(
      "AT+QCCID", std::chrono::seconds(4));
  const std::string iccid = parse_iccid_message(answer);
  shared_memory.construct<ipc_string>("iccid")(
      iccid, shared_memory.get_segment_manager());
}

std::string iccid_reader::parse_iccid_message(
    const std::string &to_parse) const {
  if (!boost::algorithm::contains(to_parse, "QCCID")) {
    throw std::runtime_error(
        std::string()
            .append("iccid_reader, could not parse iccid message: ")
            .append(to_parse));
  }
  std::vector<std::string> split_lines;
  boost::algorithm::split(split_lines, to_parse, boost::algorithm::is_space(),
                          boost::algorithm::token_compress_on);
  return parse_iccid_lines(split_lines);
}

std::string iccid_reader::parse_iccid_lines(
    const std::vector<std::string> &lines) const {
  for (auto &line : lines)
    if (boost::algorithm::all(line, boost::algorithm::is_digit())) return line;
  return {};
}
