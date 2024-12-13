#ifndef LOGGING_LOGGING_HPP
#define LOGGING_LOGGING_HPP

#include "log/config.hpp"

namespace logging {

void initialize();

struct console_config {
  bool enabled;
  bool colored;
  bool timestamp;
};

struct file_config {
  std::string file;
  bool append_to_file;
  int rotation_size;
  int file_count;
};

struct journal_config {
  bool enabled;
};

/**
 * @brief initialize
 * @param file log sink to save the log to
 * @param console enable logging to console
 */
void initialize(const config::all &config_);
}  // namespace logging

#endif  // LOGGING_LOGGING_HPP
