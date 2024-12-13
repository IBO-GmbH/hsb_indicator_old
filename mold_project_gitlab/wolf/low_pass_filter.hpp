#ifndef WOLF_LOW_PASS_FILTER_HPP
#define WOLF_LOW_PASS_FILTER_HPP

#include "log/logger.hpp"

#include <boost/signals2/signal.hpp>
#include <chrono>

namespace wolf {

class low_pass_filter {
 public:
  struct low_pass_filter_data {
    const float& filter_constant;
    const float& threshold;
    const float& threshold_with_time;
    const std::chrono::seconds& max_age;
  };

  low_pass_filter(const low_pass_filter_data& low_pass_filter_data);

  bool handle_value(const float& value);
  bool handle_value(const float& value,
                    const std::chrono::steady_clock::time_point& now);
  boost::optional<float> get_filtered_value() const;

  boost::signals2::signal<void(float)> signal_change;

 private:
  void trigger(const float& value);

 private:
  logging::logger m_logger;

  const float m_filter_constant;
  const float m_threshold;
  const float m_threshold_with_time;
  const std::chrono::seconds m_max_age;

  struct value_container {
    float filtered;
    float last_triggered;
    std::chrono::steady_clock::time_point time_last_triggered;
  };
  boost::optional<value_container> m_value;
};
}  // namespace wolf

#endif  // WOLF_LOW_PASS_FILTER_HPP
