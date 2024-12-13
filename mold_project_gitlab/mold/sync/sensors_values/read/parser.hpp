#ifndef MOLD_SYNC_SENSORS_VALUES_READ_PARSER_HPP
#define MOLD_SYNC_SENSORS_VALUES_READ_PARSER_HPP

#include "wolf/network/bond_parser.hpp"
#include "wolf/network/response/send_helper.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/sensor_value.hpp"

namespace mold::sync::sensors_values::read {

class parser {
 public:
  parser(wolf::network::bond_parser& bond_parser_,
         wolf::network::response::sender& response,
         wolf::profile_handler& profiles);

  using values_type = std::vector<wolf::sensor_value>;
  boost::signals2::signal<void(wolf::network::response::send_helper&,
                               const values_type&)>
      signal_parsed;

 private:
  bool handle_with_catch(const Clima::Message& message);
  bool handle(const Clima::Message& message);
  parser::values_type cast_from_network(
      const std::vector<::Clima::SensorValues>& to_cast);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  wolf::network::response::sender& m_response;
  wolf::profile_handler& m_profiles;
};
}  // namespace mold::sync::sensors_values::read

#endif
