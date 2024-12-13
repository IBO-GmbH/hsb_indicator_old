#ifndef RSSI_READER_HPP
#define RSSI_READER_HPP

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/signals2/signal.hpp>
#include "at_read_write.hpp"
#include "log/logger.hpp"

class rssi_reader {
 public:
  rssi_reader(at_read_write& read_write);
  ~rssi_reader() = default;
  void construct_memory(
      boost::interprocess::managed_shared_memory& shared_memory_);
  void get_next();

  boost::signals2::signal<void()> signal_read;

 private:
  void handle_read_header();
  void handle_read_body();
  std::int32_t parse_csq_message(const std::string& to_parse) const;
  std::int32_t parse_csq_line(std::string& line) const;

 private:
  mutable logging::logger m_logger;
  at_read_write& read_write;
  std::shared_ptr<std::int32_t> rssi_memory;
};

#endif  // RSSI_READER_HPP
