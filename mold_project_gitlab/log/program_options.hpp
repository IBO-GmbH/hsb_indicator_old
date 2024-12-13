#ifndef LOGGING_PROGRAM_OPTIONS_HPP
#define LOGGING_PROGRAM_OPTIONS_HPP

#include <boost/program_options/options_description.hpp>
#include "log/config.hpp"

namespace logging {

class program_options {
 public:
  program_options(config::all &config);
  boost::program_options::options_description create_description();

 private:
  config::all &m_config;
};

}  // namespace logging

#endif
