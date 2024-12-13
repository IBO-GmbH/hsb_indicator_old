#ifndef MOLD_SURFACE_TEMPERATURE_CALCULATOR_HPP
#define MOLD_SURFACE_TEMPERATURE_CALCULATOR_HPP

#include "mold/configuration_handler.hpp"
#include "mold/options_handler.hpp"

namespace mold {

class surface_temperature_calculator {
 public:
  surface_temperature_calculator(configuration_handler& configuration_handler_,
                                 options_handler& options_handler_);
  struct temperatures {
    const float indoor;
    const float outdoor;
  };
  float calculate(const wolf::types::uuid_array& configuration_id,
                  const temperatures& values) const;
  virtual ~surface_temperature_calculator() = default;

 protected:
  virtual float get_threshold_factor(
      const wolf::types::uuid_array& configuration_id) const;

  configuration_handler& m_configuration_handler_;
  options_handler& m_options_handler;
};

}  // namespace mold

#endif
