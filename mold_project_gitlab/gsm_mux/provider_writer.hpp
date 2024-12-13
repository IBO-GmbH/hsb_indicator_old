#ifndef PROVIDER_WRITER_HPP
#define PROVIDER_WRITER_HPP

#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <string>
#include "log/logger.hpp"

class provider_writer {
 public:
  using char_allocator = boost::interprocess::allocator<
      char, boost::interprocess::managed_shared_memory::segment_manager>;
  using ipc_string =
      boost::interprocess::basic_string<char, std::char_traits<char>,
                                        char_allocator>;

 public:
  provider_writer();

  void construt_memory(
      boost::interprocess::managed_shared_memory& shared_memory);
  void write(const std::string& provider);

 private:
  mutable logging::logger m_logger;
  std::shared_ptr<ipc_string> provider_memory;
  std::shared_ptr<char_allocator> allocator;
};

#endif  // PROVIDER_WRITER_HPP
