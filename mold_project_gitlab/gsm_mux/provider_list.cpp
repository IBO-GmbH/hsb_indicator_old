#include "provider_list.hpp"
#include <boost/regex.hpp>
#include "string_print_helper.hpp"

using namespace logging;

provider_list::provider_list(at_read_write &read_write_)
    : m_logger{"provder_list"}, read_write(read_write_) {}

void provider_list::refresh() {
  read_write.writer.write("AT+COPS=?");
  read_write.reader.read_till_ok([this] { handle_at_read(); },
                                 std::chrono::seconds(90));
}

void provider_list::handle_at_read() {
  // TODO check for error
  const std::string result = read_write.reader.get_result();
  LOGGING_LOG(m_logger, severity::normal)
      << "handle_at_read:" << string_print_helper::from_at_string(result);
  items.clear();
  parse_cops(result);
  signal_refreshed();
}

void provider_list::parse_cops(const std::string &parse) {
  // https://regex101.com/r/mCY6xJ/8
  // parses COPS: (1,"A1","A1","23201"),(1,"Orange","Orange A","23205") ... to
  // tuple elements
  static const boost::regex expression(
      R"regex(\((?<stat>[0-3]),"(?<name>.*?)","(?<name_short>.*?)","(?<operator>.*?)"\))regex");
  boost::sregex_iterator iter(parse.begin(), parse.end(), expression);
  boost::sregex_iterator end;
  for (; iter != end; ++iter) {
    //    LOGGING_LOG(m_logger, severity::normal) << (*iter) ;
    item result;
    result.status = static_cast<stat>(std::stoi((*iter)["stat"].str()));
    result.name = (*iter)["name"];
    result.name_short = (*iter)["name_short"];
    result.operator_ = (*iter)["operator"];
    items.push_back(result);
  }
  std::stringstream output;
  std::copy(items.cbegin(), items.cend(),
            std::ostream_iterator<item>(output, ", "));
  LOGGING_LOG(m_logger, severity::normal)
      << "parse_cops, items:" << output.str();
}

std::ostream &operator<<(std::ostream &out, const provider_list::stat print) {
  switch (print) {
    case provider_list::stat::available:
      return out << "available";
    case provider_list::stat::current:
      return out << "current";
    case provider_list::stat::forbidden:
      return out << "forbidden";
    case provider_list::stat::unknown:
      return out << "unknown";
  }
  assert(false);
  return out << "failure";
}

std::ostream &operator<<(std::ostream &out, const provider_list::item print) {
  return out << "(status:" << print.status << ", name:" << print.name
             << ", name_short:" << print.name_short
             << ", operator:" << print.operator_ << ")";
}
