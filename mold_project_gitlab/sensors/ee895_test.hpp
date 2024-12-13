#ifndef EE895_TEST_HPP
#define EE895_TEST_HPP

#include "wolf/ee895_parser.hpp"
#include "wolf/ee895_reader.hpp"

class ee895_test {
 public:
  ee895_test(wolf::ee895_reader& reader);
  ~ee895_test();

 private:
  void handle_read(const wolf::ee895_reader::read_buffer& data);

 private:
  logging::logger m_logger;

  wolf::ee895_reader& m_reader;
  wolf::ee895_parser m_parser;

  const std::chrono::seconds m_read_interval{15};
};

#endif  // EE895_TEST_HPP
