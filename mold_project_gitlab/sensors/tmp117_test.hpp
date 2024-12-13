#ifndef TMP117_TEST_HPP
#define TMP117_TEST_HPP

#include "wolf/tmp117_parser.hpp"
#include "wolf/tmp117_reader.hpp"
#include "wolf/types.hpp"

class tmp117_test {
 public:
  tmp117_test(wolf::tmp117_reader& reader);
  ~tmp117_test();

 private:
  void handle_read(const wolf::types::data& data);

 private:
  logging::logger m_logger;

  wolf::tmp117_reader& m_reader;
  wolf::tmp117_parser m_parser;

  const std::chrono::seconds m_read_interval{10};
};

#endif  // TMP117_TEST_HPP
