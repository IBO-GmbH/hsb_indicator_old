#include "median_calculator_creator.hpp"

using namespace mold;

median_calculator_creator::median_calculator_creator(const config config)
    : m_config(config) {}

median_calculator median_calculator_creator::create() const {
  return median_calculator{m_config};
}
