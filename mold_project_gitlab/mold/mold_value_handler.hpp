#ifndef MOLD_VALUE_HANDLER_HPP
#define MOLD_VALUE_HANDLER_HPP

#include "mold/mold_value.hpp"
#include "mold/program_options_parser.hpp"
#include "mold/sprout/handler.hpp"
#include "wolf/low_pass_filter_creator.hpp"
#include "wolf/sensor_filter.hpp"

namespace mold {

class mold_value_handler {
 public:
  using time_point = std::chrono::system_clock::time_point;

  mold_value_handler(sprout::handler& sprout_,
                     const mold::config& program_options);
  void recalculate(const wolf::types::uuid_array& config_id,
                   const time_point& now);
  using values = std::vector<mold_value>;
  const values& get_all() const;
  boost::signals2::signal<void(const mold_value&)> signal_value;

 private:
  void calculate_and_save(const sprout::data_types::value& from,
                          const time_point& now);
  mold_value calculate(const sprout::data_types::value& from,
                       const time_point& now) const;
  values::iterator find(const wolf::types::uuid_array& id);
  wolf::low_pass_filter& create_or_get(
      const wolf::types::uuid_array& configuration);

  mutable logging::logger m_logger;
  sprout::handler& m_sprout;
  values m_values;
  wolf::low_pass_filter_creator m_filter_creator;
  wolf::sensor_filter m_filter_handler;
  using filter_entry =
      std::pair<wolf::types::uuid_array, wolf::low_pass_filter>;
  using filters = std::vector<filter_entry>;
  filters m_filters;
};

}  // namespace mold

#endif  // MOLD_VALUE_HANDLER_HPP
