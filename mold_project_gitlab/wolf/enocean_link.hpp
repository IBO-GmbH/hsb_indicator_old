#ifndef WOLF_ENOCEAN_LINK_HPP
#define WOLF_ENOCEAN_LINK_HPP

#include <Includes/eoChannelEnums.h>
#include <boost/signals2/signal.hpp>
#include <unordered_map>

#include "eoLink.h"
#include "log/logger.hpp"
#include "wolf/enocean_subchannel_type.hpp"
#include "wolf/types.hpp"

class eoGateway;
class eoProfile;

namespace wolf {

class enocean_link {
 public:
  using eep = std::array<uint8_t, 3>;
  using optional_subchannel = std::optional<enocean_subchannel_type>;

  enocean_link();
  ~enocean_link();

  struct register_result {
    float range_from;
    float range_to;
    std::string name;
    CHANNEL_TYPE type;
    optional_subchannel subchannel;
    std::string unit;
  };
  using register_results = std::vector<register_result>;
  // TODO return profiles
  register_results register_device(const types::id_esp3 id_, const uint8_t& org,
                                   const uint8_t& func, const uint8_t& type,
                                   const bool is_outdoor);

  struct parse_result {
    float value;
    std::string name;
    CHANNEL_TYPE type;
    optional_subchannel subchannel;
    std::string unit;
  };
  using parse_results = std::vector<parse_result>;
  parse_results parse_telegram(const types::id_esp3 id,
                               const types::data& to_parse,
                               const bool is_outdoor);

  void clear();

  using device = std::pair<types::id_esp3, eep>;
  using devices = std::vector<device>;
  const devices& get_devices();

  void remove_devices(const std::vector<types::id_esp3>& ids);

  std::optional<types::eep> get_eep_of_device(const types::id_esp3 id);

  boost::signals2::signal<void(const types::id_esp3, const parse_results&)>
      signal_parsed;

 private:
  bool learn_eep(const eep& eep, register_results& result,
                 const bool is_outdoor);
  void convert_enocean_profile_to_register_results(eoProfile* profile,
                                                   register_results& result,
                                                   const bool is_outdoor) const;
  devices::iterator find_device(const types::id_esp3 id);
  static bool is_valid(const bool is_outdoor, eoChannelInfo* channel);
  void remove_device(const types::id_esp3& id);
  using channels = std::vector<eoChannelInfo>;
  static bool is_channel_already_handled(const channels& list,
                                         const eoChannelInfo& channel);
  static bool read_value_no_subtype(eoProfile* const profile,
                                    const CHANNEL_TYPE& type,
                                    std::stringstream& out, parse_result& work);
  static bool read_value_subtype(eoProfile* const profile,
                                 const CHANNEL_TYPE& type,
                                 const std::uint8_t& subtype,
                                 std::stringstream& out, parse_result& work);
  static std::array<bool, 3> get_subchannel_states(const types::data& to_parse);
  void parse_particles(eoProfile* const profile, eoChannelInfo* channel,
                       const types::data& to_parse, std::stringstream& out,
                       const parse_result& work, parse_results& work_items);
  void parse_current(eoProfile* const profile, eoChannelInfo* channel,
                     std::stringstream& out, const parse_result& work,
                     parse_results& work_items);
  static bool is_channel_a_flag(const CHANNEL_TYPE& type);

  // we have to include special cases, because they need to be registered and
  // parsed differently
  enum special_cases {
    none,
    air_pressure_official,
    air_pressure_inofficial,
  };
  static special_cases get_special_case_type(eoProfile* const profile);
  static void register_special_case(eoProfile* const profile,
                                    register_result& work_item);
  static void parse_value_special_case(eoProfile* const profile,
                                       parse_result& work_item);

 private:
  mutable logging::logger m_logger;
  using profiles = std::unordered_map<eep, std::unique_ptr<eoProfile> >;
  profiles m_profiles;
  devices m_devices;
};
}  // namespace wolf

#endif  // WOLF_ENOCEAN_LINK_HPP
