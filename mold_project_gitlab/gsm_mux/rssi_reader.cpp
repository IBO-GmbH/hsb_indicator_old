#include "rssi_reader.hpp"
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <sstream>

using namespace logging;

rssi_reader::rssi_reader(at_read_write& read_write_)
    : m_logger{"rssi_reader"}, read_write(read_write_) {}

void rssi_reader::construct_memory(
    boost::interprocess::managed_shared_memory& shared_memory_) {
  rssi_memory.reset(shared_memory_.construct<std::int32_t>("rssi")(0));
}

void rssi_reader::get_next() {
  read_write.writer.write("AT+CSQ");
  read_write.reader.read("AT+CSQ", [this]() { handle_read_header(); },
                         std::chrono::seconds(4));
}

void rssi_reader::handle_read_header() {
  read_write.reader.get_result();
  read_write.reader.read_till_ok([this]() { handle_read_body(); });
}

void rssi_reader::handle_read_body() {
  const auto answer = read_write.reader.get_result();
  if (answer.find("OK") == std::string::npos)
    throw std::runtime_error(
        "rssi_reader::handle_read_body, could not get an \"OK\"");
  const std::int32_t rssi = parse_csq_message(answer);
  *rssi_memory = rssi;
  signal_read();
}

std::int32_t rssi_reader::parse_csq_message(const std::string& to_parse) const {
  std::istringstream input(to_parse);
  for (std::string line; std::getline(input, line);) {
    if (line.find("CSQ:") != std::string::npos) return parse_csq_line(line);
  }
  throw std::runtime_error("rssi_reader, could not parse csq message");
}

std::int32_t rssi_reader::parse_csq_line(std::string& line) const {
  LOGGING_LOG(m_logger, severity::normal)
      << "rssi_reader, parse line: '" << line << "'";
  std::istringstream input(line);
  const auto found =
      std::find_if(line.cbegin(), line.cend(),
                   [](unsigned char check) { return std::isdigit(check); });
  if (found == line.cend()) return -1;
  const auto offset = std::distance(line.cbegin(), found);
  input.seekg(offset);
  std::int32_t result{};
  input >> result;
  return result;
}
