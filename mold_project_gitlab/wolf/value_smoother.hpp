#ifndef WOLF_VALUE_SMOOTHER_HPP
#define WOLF_VALUE_SMOOTHER_HPP

#include "log/logger.hpp"

#include <boost/signals2/signal.hpp>

namespace wolf {

/*
 * @brief The value_smoother class takes a user defined count of values,
 * smoothes them and calls and triggers an event
 */
class value_smoother {
 public:
  value_smoother(const int values_to_collect);

  boost::optional<float> handle(const float value);
  boost::signals2::signal<void(float)> signal_smoothed_value;

 private:
  float calculate_average() const;

 private:
  logging::logger m_logger;

  const std::size_t m_values_to_collect;
  std::vector<float> m_collection_of_values;
};
}

#endif  // WOLF_VALUE_SMOOTHER_HPP
