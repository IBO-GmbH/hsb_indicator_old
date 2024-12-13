#ifndef AT_READ_WRITE_HPP
#define AT_READ_WRITE_HPP

#include <boost/asio/io_service.hpp>
#include "at_reader.hpp"
#include "at_writer.hpp"

class at_read_write {
 public:
  at_read_write(boost::asio::serial_port& serial, const std::string& info);

  at_reader reader;
  at_writer writer;
};

#endif  // AT_READ_WRITE_HPP
