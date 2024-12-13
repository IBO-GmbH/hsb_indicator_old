#ifndef SHTC1_TEST_HPP
#define SHTC1_TEST_HPP

#include "wolf/shtc1_parser.hpp"
#include "wolf/shtc1_reader.hpp"
#include "wolf/types.hpp"

class shtc1_test {
 public:
  shtc1_test(wolf::shtc1_reader& reader);
  ~shtc1_test();

 private:
  void handle_read(const wolf::types::data& data);

 private:
  logging::logger m_logger;

  wolf::shtc1_reader& m_reader;
  wolf::shtc1_parser m_parser;

  const std::chrono::seconds m_read_interval{10};
};

#endif  // SHTC1_TEST_HPP
