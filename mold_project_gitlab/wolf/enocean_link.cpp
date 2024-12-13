#include "enocean_link.hpp"
#include <cmath>
#include "eoprofile_stm550.hpp"
#include "wolf/enocean_channel_helper.hpp"
#include "wolf/sensor_id_enocean.hpp"

using namespace logging;
using namespace wolf;

enocean_link::enocean_link() : m_logger{"enocean_link"} {}

enocean_link::~enocean_link() {}

enocean_link::register_results enocean_link::register_device(
    const types::id_esp3 id, const uint8_t &org, const uint8_t &func,
    const uint8_t &type, const bool is_outdoor) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "registering device with esp3-id:" << std::hex << id;

  const eep eep_{org, func, type};
  register_results result;
  if (!learn_eep(eep_, result, is_outdoor)) {
    return register_results();
  }
  auto found = find_device(id);
  if (found != m_devices.cend()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "device with esp3-id:0x" << std::hex << id
        << " is already registered, overwritting";
    found->second = std::move(eep_);
  } else {
    m_devices.emplace_back(std::make_pair(id, std::move(eep_)));
  }

  LOGGING_LOG(m_logger, severity::normal)
      << "successfully registered:0x" << std::hex << id
      << " resulting values-types size():" << std::dec << result.size();

  return result;
}

enocean_link::parse_results enocean_link::parse_telegram(
    const types::id_esp3 id, const types::data &to_parse,
    const bool is_outdoor) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "enocean_link::parse_telegram, id:" << std::hex << id;

  auto found = find_device(id);
  if (found == m_devices.cend()) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "could not parse_telegram because not registered yet, id:"
        << std::hex << id;
    return parse_results();
  }
  const eep eep = found->second;
  profiles::const_iterator it_eep = m_profiles.find(eep);
  if (it_eep == m_profiles.cend()) {
    LOGGING_LOG(m_logger, severity::error)
        << "FATAL! it_eep == m_profiles.cend()";
    throw std::runtime_error("enocean_link::parse_telegram: eep not found");
  }
  eoProfile *const profile = it_eep->second.get();

  eoMessage message(static_cast<uint16_t>(to_parse.size()));
  message.RORG = it_eep->first[0];
  message.SetDataLength(static_cast<uint16_t>(to_parse.size()));
  std::copy(to_parse.cbegin(), to_parse.cend(), message.data);

  LOGGING_LOG(m_logger, severity::verbose)
      << "to_parse.size():" << std::dec << to_parse.size()
      << ", to_parse:" << to_parse;

  eoReturn result_parse = profile->Parse(message);
  if (result_parse != EO_OK) {
    LOGGING_LOG(m_logger, severity::error)
        << "result != EO_OK when trying to parse eoMessage!,"
           " result:"
        << std::dec << result_parse;
    return parse_results();
  }

  parse_results result;

  channels parsed_channels;
  for (int ind_channel = 0; ind_channel < profile->GetChannelCount();
       ind_channel++) {
    eoChannelInfo *channel =
        profile->GetChannel(static_cast<uint8_t>(ind_channel));

    if (is_channel_already_handled(parsed_channels, *channel) ||
        is_channel_a_flag(channel->type))
      continue;

    parsed_channels.push_back(*channel);

    if (!is_valid(is_outdoor, channel)) continue;

    parse_result work;
    work.name = std::string(channel->ToString(NAME));
    work.unit = std::string(channel->ToString(UNIT));
    work.type = channel->type;
    work.subchannel = optional_subchannel();

    std::stringstream out;

    out << "name: " << work.name << ", unit: " << work.unit;

    parse_results work_items;
    if (channel->type == S_PARTICLES) {
      parse_particles(profile, channel, to_parse, out, work, work_items);
    } else if (channel->type == S_CURRENT) {
      parse_current(profile, channel, out, work, work_items);
    } else {
      if (read_value_no_subtype(profile, channel->type, out, work))
        work_items.push_back(std::move(work));
    }
    LOGGING_LOG(m_logger, severity::normal) << out.str();

    for (auto &work_item : work_items) result.push_back(std::move(work_item));
  }

  LOGGING_LOG(m_logger, severity::verbose)
      << "successfully parsed resulting values size():" << std::dec
      << result.size();

  signal_parsed(id, result);

  return result;
}

void enocean_link::clear() {
  m_profiles.clear();
  m_devices.clear();
}

const enocean_link::devices &enocean_link::get_devices() { return m_devices; }

void enocean_link::remove_devices(const std::vector<types::id_esp3> &ids) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "remove " << ids.size() << " devices";
  for (const auto &id : ids) remove_device(id);
}

std::optional<types::eep> enocean_link::get_eep_of_device(
    const types::id_esp3 id) {
  auto found = find_device(id);
  if (found == m_devices.cend()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "device with esp3-id:0x" << std::hex << id << " was not found";
    return std::optional<types::eep>();
  }
  return found->second;
}

void enocean_link::remove_device(const types::id_esp3 &id) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "remove device with esp3-id:" << std::hex << id;

  auto found = find_device(id);
  if (found == m_devices.cend()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "device with esp3-id:0x" << std::hex << id
        << " can not be removed, not found";
    return;
  }
  m_devices.erase(found);
}

bool enocean_link::is_channel_already_handled(
    const enocean_link::channels &list, const eoChannelInfo &channel) {
  const auto found =
      std::find_if(list.begin(), list.end(), [&channel](const auto &to_check) {
        return channel.type == to_check.type;
      });
  return found != list.end();
}

bool enocean_link::read_value_no_subtype(eoProfile *const profile,
                                         const CHANNEL_TYPE &type,
                                         stringstream &out,
                                         enocean_link::parse_result &work) {
  float float_value;
  uint8_t uint8_value;
  if (profile->GetValue(type, float_value) == EO_OK) {
    if (type == S_LUMINANCE) float_value = std::floor(float_value);
    work.value = float_value;
    parse_value_special_case(profile, work);
    out << ", read float_value: " << work.value;
    return true;
  }
  if (profile->GetValue(type, uint8_value) == EO_OK) {
    work.value = uint8_value;
    parse_value_special_case(profile, work);
    out << ", read uint8_value: " << work.value;
    return true;
  }
  return false;
}

bool enocean_link::read_value_subtype(eoProfile *const profile,
                                      const CHANNEL_TYPE &type,
                                      const uint8_t &subtype, stringstream &out,
                                      enocean_link::parse_result &work) {
  float float_value;
  uint8_t uint8_value;
  if (profile->GetValue(type, float_value, subtype) == EO_OK) {
    out << ", read float_value: " << float_value;
    work.value = float_value;
    return true;
  }
  if (profile->GetValue(type, uint8_value, subtype) == EO_OK) {
    out << ", read uint8_value: " << uint8_value;
    work.value = uint8_value;
    return true;
  }
  return false;
}

std::array<bool, 3> enocean_link::get_subchannel_states(
    const types::data &to_parse) {
  std::array<bool, 3> sub_types;
  const auto last_byte = to_parse.back();
  sub_types[0] = static_cast<bool>(last_byte & 0x04);
  sub_types[1] = static_cast<bool>(last_byte & 0x02);
  sub_types[2] = static_cast<bool>(last_byte & 0x01);
  return sub_types;
}

void enocean_link::parse_particles(eoProfile *const profile,
                                   eoChannelInfo *channel,
                                   const types::data &to_parse,
                                   stringstream &out, const parse_result &work,
                                   parse_results &work_items) {
  std::vector<eoEEPChannelInfo *> sub_types;
  profile->GetSubTypeChannel(channel->type, sub_types);
  std::array<bool, 3> sub_channel_states = get_subchannel_states(to_parse);
  const auto subchannel_offset =
      3 * static_cast<int>(types_with_subchannel::particles);
  if (sub_types.size() != sub_channel_states.size()) {
    LOGGING_LOG(m_logger, severity::error)
        << "particles did not deliver 3 sub_types!";
    return;
  }
  for (unsigned int index = 0; index < sub_types.size(); ++index) {
    if (!sub_channel_states[index]) continue;
    auto work_copy = work;
    work_copy.subchannel = static_cast<enocean_subchannel_type>(
        (sub_types[index]->eepItem->index + subchannel_offset));
    out << ", subchannel: " << work_copy.subchannel.value();
    if (read_value_subtype(profile, channel->type,
                           sub_types[index]->eepItem->index, out, work_copy))
      work_items.push_back(std::move(work_copy));
  }
}

void enocean_link::parse_current(eoProfile *const profile,
                                 eoChannelInfo *channel, stringstream &out,
                                 const enocean_link::parse_result &work,
                                 enocean_link::parse_results &work_items) {
  std::vector<eoEEPChannelInfo *> sub_types;
  profile->GetSubTypeChannel(channel->type, sub_types);
  const auto subchannel_offset =
      3 * static_cast<int>(types_with_subchannel::current);
  for (unsigned int index = 0; index < sub_types.size(); ++index) {
    auto work_copy = work;
    work_copy.subchannel = static_cast<enocean_subchannel_type>(
        (sub_types[index]->eepItem->index + subchannel_offset));
    out << ", subchannel: " << work_copy.subchannel.value();
    if (read_value_subtype(profile, channel->type,
                           sub_types[index]->eepItem->index, out, work_copy))
      work_items.push_back(std::move(work_copy));
  }
}

bool enocean_link::is_channel_a_flag(const CHANNEL_TYPE &type) {
  // we are allowing this flag for the motion sensor
  if (type == 0xf09) return false;
  return (type & T_FLAG) == T_FLAG;
}

enocean_link::special_cases enocean_link::get_special_case_type(
    eoProfile *const profile) {
  if (profile->rorg == 0xa5)
    if (profile->func == 0x05) {
      // the official barometer is handled wrong by the old version of the
      // enocean library. the values and the range should be in hPa, but the
      // unit description is Pa and the values do not match either one of the
      // units.
      if (profile->type == 0x01) return special_cases::air_pressure_official;
      // the inofficial barometer, the one CW built himself, uses an inofficial
      // EEP but gets parsed and registered fine nonetheless. the sent values
      // are in Pa, but to unify it with the official barometer we convert them
      // to hPa.
      else if (profile->type == 0x0a)
        return special_cases::air_pressure_inofficial;
    }
  return special_cases::none;
}

void enocean_link::register_special_case(
    eoProfile *const profile, enocean_link::register_result &work_item) {
  const auto special_type = get_special_case_type(profile);
  switch (special_type) {
    case special_cases::air_pressure_official:
      if (work_item.type == S_PRESSURE) {
        work_item.unit = "hecto Pascals";
        work_item.range_from *= 100;
        work_item.range_to *= 100;
      }
      return;
    case special_cases::air_pressure_inofficial:
      if (work_item.type == S_PRESSURE) {
        work_item.unit = "hecto Pascals";
        work_item.range_from /= 100;
        work_item.range_to /= 100;
      }
      return;
    default:
      return;
  }
}

void enocean_link::parse_value_special_case(eoProfile *const profile,
                                            parse_result &work_item) {
  const auto special_type = get_special_case_type(profile);
  switch (special_type) {
    case special_cases::air_pressure_official:
      if (work_item.type == S_PRESSURE) {
        work_item.unit = "hecto Pascals";
        work_item.value *= 100;
      }
      return;
    case special_cases::air_pressure_inofficial:
      if (work_item.type == S_PRESSURE) {
        work_item.unit = "hecto Pascals";
        work_item.value /= 100;
      }
      return;
    default:
      return;
  }
}

bool enocean_link::learn_eep(const eep &eep, register_results &result,
                             const bool is_outdoor) {
  logger &logger_local_scope = m_logger;
  auto print_eep = [&eep, &logger_local_scope](const severity &severity,
                                               const std::string &message) {
    LOGGING_LOG(logger_local_scope, severity)
        << message << "rorg:0x" << std::hex << static_cast<int>(eep[0])
        << " func:0x" << static_cast<int>(eep[1]) << " type:0x"
        << static_cast<int>(eep[2]);
  };

  print_eep(severity::verbose, "learning profile, eep:");

  profiles::const_iterator it_profile = m_profiles.find(eep);
  if (it_profile != m_profiles.cend()) {
    print_eep(severity::verbose, "eep-profile already learned, ");

    convert_enocean_profile_to_register_results(it_profile->second.get(),
                                                result, is_outdoor);

    return true;
  }
  // create profile
  eoProfile *profile = eoProfileFactory::CreateProfile(eep[0], eep[1], eep[2]);
  if (profile == nullptr) {
    if (eep[0] == 0xd2 && eep[1] == 0x14 && eep[2] == 0x41)
      profile = new eoProfile_stm550(eep[0], eep[1], eep[2]);
    else {
      print_eep(severity::error, "eoProfileFactory could not create profile, ");
      return false;
    }
  }
  print_eep(severity::normal,
            "eoProfileFactory learned and created a new profile, ");

  convert_enocean_profile_to_register_results(profile, result, is_outdoor);

  m_profiles[eep] = std::unique_ptr<eoProfile>(profile);

  return true;
}

void enocean_link::convert_enocean_profile_to_register_results(
    eoProfile *profile, enocean_link::register_results &result,
    const bool is_outdoor) const {
  channels converted_channels;
  for (int ind_channel = 0; ind_channel < profile->GetChannelCount();
       ++ind_channel) {
    eoChannelInfo *channel =
        profile->GetChannel(static_cast<uint8_t>(ind_channel));
    if (is_channel_already_handled(converted_channels, *channel) ||
        is_channel_a_flag(channel->type))
      continue;

    converted_channels.push_back(*channel);

    if (!is_valid(is_outdoor, channel)) continue;
    register_result work;
    work.name = std::string(channel->ToString(NAME));
    work.unit = std::string(channel->ToString(UNIT));
    work.range_from = channel->min;
    work.range_to = channel->max;
    work.type = channel->type;
    work.subchannel = optional_subchannel();

    register_special_case(profile, work);

    LOGGING_LOG(m_logger, severity::normal)
        << "channel[" << std::dec << ind_channel << "] unit:\"" << work.unit
        << "\" name:\"" << work.name << "\" range:[" << work.range_from << ", "
        << work.range_to << "] type:" << std::hex
        << static_cast<int>(work.type);

    register_results work_items;
    const auto type_with_subchannels =
        enocean_channel_helper::get_type_with_subchannel(channel->type);
    if (type_with_subchannels.has_value()) {
      std::vector<eoEEPChannelInfo *> sub_types;
      profile->GetSubTypeChannel(channel->type, sub_types);
      const auto subchannel_offset =
          3 * static_cast<int>(type_with_subchannels.value());
      for (const auto &subtype : sub_types) {
        auto work_copy = work;
        work_copy.subchannel = static_cast<enocean_subchannel_type>(
            (subtype->eepItem->index + subchannel_offset));
        LOGGING_LOG(m_logger, severity::normal)
            << "subchannel: " << work_copy.subchannel.value();
        work_items.push_back(std::move(work_copy));
      }
    } else
      work_items.push_back(std::move(work));
    for (auto &work_item : work_items) result.push_back(std::move(work_item));
  }
}

enocean_link::devices::iterator enocean_link::find_device(
    const types::id_esp3 id) {
  return std::find_if(
      m_devices.begin(), m_devices.end(),
      [&id](const device &device) { return device.first == id; });
}

bool enocean_link::is_valid(const bool is_outdoor, eoChannelInfo *channel) {
  if (!is_outdoor) return true;  // indoor sensors are always valid
  return channel->type == S_TEMP ||
         channel->type == S_RELHUM;  // outdoor only valid if temp or hum
}
