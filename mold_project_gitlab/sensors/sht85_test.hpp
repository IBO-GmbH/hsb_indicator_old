#ifndef SHT85_TEST_HPP
#define SHT85_TEST_HPP

#include "wolf/sht85_parser.hpp"
#include "wolf/sht85_reader.hpp"
#include "wolf/types.hpp"

class sht85_test {
 public:
  sht85_test(wolf::sht85_reader& reader);
  ~sht85_test();

 private:
  void handle_read(const wolf::types::data& data);

 private:
  logging::logger m_logger;

  wolf::sht85_reader& m_reader;
  wolf::sht85_parser m_parser;

  const std::chrono::seconds m_read_interval{10};
};

#endif  // SHT85_TEST_HPP
