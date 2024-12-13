#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "config.hpp"

class options {
 public:
  options();
  bool parse(int argc, char* argv[]);

  config::all config_;
};

#endif  // OPTIONS_HPP
