#include "value_smoother.hpp"
#include <numeric>

using namespace logging;
using namespace wolf;

value_smoother::value_smoother(const int values_to_collect)
    : m_logger("value_smoother"),
      m_values_to_collect(
          static_cast<std::size_t>(std::max(values_to_collect, 1))) {
  m_collection_of_values.reserve(m_values_to_collect);
}

boost::optional<float> value_smoother::handle(const float value) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle, value:" << value
      << " m_collection_of_values.size():" << m_collection_of_values.size();
  m_collection_of_values.push_back(value);
  if (m_collection_of_values.size() < m_values_to_collect) return {};
  const float result = calculate_average();
  signal_smoothed_value(result);
  m_collection_of_values.clear();
  return result;
}

float value_smoother::calculate_average() const {
  const float values_sum = std::accumulate(m_collection_of_values.cbegin(),
                                           m_collection_of_values.cend(), 0.f);
  return values_sum / static_cast<float>(m_collection_of_values.size());
}
