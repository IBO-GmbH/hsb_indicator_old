#ifndef ICCID_READER_HPP
#define ICCID_READER_HPP

#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include "command_handler.hpp"
#include "log/logger.hpp"

class iccid_reader {
 public:
  using char_allocator = boost::interprocess::allocator<
      char, boost::interprocess::managed_shared_memory::segment_manager>;
  using ipc_string =
      boost::interprocess::basic_string<char, std::char_traits<char>,
                                        char_allocator>;

  iccid_reader(command_handler& command_);
  ~iccid_reader() = default;
  void read(boost::interprocess::managed_shared_memory& shared_memory);

 private:
  std::string parse_iccid_message(const std::string& to_parse) const;
  std::string parse_iccid_lines(const std::vector<std::string>& lines) const;

 private:
  mutable logging::logger m_logger;
  command_handler& command;
};

#endif  // ICCID_READER_HPP
