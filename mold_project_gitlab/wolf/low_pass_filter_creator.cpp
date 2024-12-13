#include "low_pass_filter_creator.hpp"

using namespace wolf;

low_pass_filter_creator::low_pass_filter_creator(
    const low_pass_filter::low_pass_filter_data& low_pass_filter_data)
    : m_filter_constant{low_pass_filter_data.filter_constant},
      m_threshold{low_pass_filter_data.threshold},
      m_threshold_with_time{low_pass_filter_data.threshold_with_time},
      m_max_age{low_pass_filter_data.max_age} {}

low_pass_filter low_pass_filter_creator::create() const {
  return low_pass_filter{
      {m_filter_constant, m_threshold, m_threshold_with_time, m_max_age}};
}
