#ifndef LOGGING_CONFIG_HPP
#define LOGGING_CONFIG_HPP

#include "log/severity.hpp"

namespace logging::config {

struct common {
  logging::severity severity{logging::severity::verbose};
  bool flush{true};
};

struct console {
  bool enabled{};
  bool color{true};
  bool timestamp{true};
};

struct file {
  std::string file;
  bool append{true};
  int rotation_size{1};
  int file_count{5};
};

struct journal {
  bool enabled{true};
};

struct all {
  common common_;
  console console_;
  file file_;
  journal journal_;
};

}  // namespace logging::config

#endif
