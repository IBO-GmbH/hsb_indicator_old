#ifndef MOLD_MEDIAN_CALCULATOR_HPP
#define MOLD_MEDIAN_CALCULATOR_HPP

#include "log/logger.hpp"
#include "mold/program_options_parser.hpp"

#include <chrono>
#include <deque>

namespace mold {

class median_calculator {
 public:
  using timepoint = std::chrono::system_clock::time_point;

  struct value {
    float value;
    timepoint timestamp;
  };

  using values = std::deque<value>;

  median_calculator(const mold::config& config);
  float calculate_median(const float value, const timepoint& timepoint_);

 private:
  float get_median_value(const float value);
  void delete_old_values(const timepoint& now);
  void sort_values();

 private:
  logging::logger m_logger;
  values m_values;
  bool m_enabled;
  std::chrono::minutes m_median_time;
};
}  // namespace mold

#endif  // MOLD_MEDIAN_CALCULATOR_HPP
