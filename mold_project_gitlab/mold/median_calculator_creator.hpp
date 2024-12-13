#ifndef MOLD_MEDIAN_CALCULATOR_CREATOR_HPP
#define MOLD_MEDIAN_CALCULATOR_CREATOR_HPP

#include "mold/median_calculator.hpp"

namespace mold {

class median_calculator_creator {
 public:
  median_calculator_creator(const mold::config config);
  median_calculator create() const;

 private:
  const mold::config m_config;
};

}  // namespace mold

#endif
