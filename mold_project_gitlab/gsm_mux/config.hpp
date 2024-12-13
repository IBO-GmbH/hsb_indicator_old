#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include "log/config.hpp"

namespace config {
struct general {
  int pin;
  std::string device;
  std::string online_check_url;
  bool run_pppd;
  std::string isp_name;
};
struct all {
  general general_;
  logging::config::all log;
};
}  // namespace config

#endif  // CONFIG_HPP
