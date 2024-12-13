#ifndef WOLF_LOW_PASS_FILTER_CREATOR_HPP
#define WOLF_LOW_PASS_FILTER_CREATOR_HPP

#include "low_pass_filter.hpp"

#include <chrono>

namespace wolf {

class low_pass_filter_creator {
 public:
  low_pass_filter_creator(
      const low_pass_filter::low_pass_filter_data& low_pass_filter_data);

  low_pass_filter create() const;

 private:
  const float m_filter_constant;
  const float m_threshold;
  const float m_threshold_with_time;
  const std::chrono::seconds m_max_age;
};
}  // namespace wolf

#endif  // WOLF_LOW_PASS_FILTER_CREATOR_HPP
