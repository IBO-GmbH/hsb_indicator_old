#include "heating_valve_settings_cache.hpp"
#include "wolf/base64.hpp"

namespace soci {
template <>
struct type_conversion<mold::heating_valve_settings> {
  using base_type = values;

  static void from_base(values const &values_, indicator /* ind */,
                        mold::heating_valve_settings &to_cast) {
    wolf::types::id id =
        wolf::base64::decode(values_.get<std::string>("room_uuid"));
    if (id.size() != 16)
      throw std::runtime_error(
          "type_conversion<heating_valve_settings>::from_base: id not 16 "
          "signs");
    std::copy(id.cbegin(), id.cend(), to_cast.room_id.begin());
    to_cast.manual_mode =
        (values_.get<std::string>("manual_mode", "") == "0" ? false : true);
    to_cast.summer_mode =
        (values_.get<std::string>("summer_mode", "") == "0" ? false : true);
    to_cast.desired_temperature =
        std::stof(values_.get<std::string>("desired_temperature", ""));
    to_cast.set_valve_position =
        std::stoi(values_.get<std::string>("set_valve_position", ""));
  }

  static void to_base(const mold::heating_valve_settings &to_cast,
                      values &values_, indicator &ind) {
    values_.set("room_uuid", wolf::base64::encode(to_cast.room_id));
    const std::string manual_mode = (to_cast.manual_mode ? "1" : "0");
    values_.set("manual_mode", manual_mode);
    const std::string summer_mode = (to_cast.summer_mode ? "1" : "0");
    values_.set("summer_mode", summer_mode);
    values_.set("desired_temperature",
                std::to_string(to_cast.desired_temperature));
    values_.set("set_valve_position",
                std::to_string(to_cast.set_valve_position));

    ind = i_ok;
  }
};
}  // namespace soci

mold::heating_valve_settings_cache::heating_valve_settings_cache(
    wolf::database &database)
    : m_logger{"heating_valve_settings_cache"},
      m_sync(m_logger, database, "heating_valve_settings",
             {{"room_uuid", "text"},
              {"manual_mode", "text"},
              {"summer_mode", "text"},
              {"desired_temperature", "text"},
              {"set_valve_position", "text"}}) {}

void mold::heating_valve_settings_cache::add(
    const mold::heating_valve_settings &to_add) {
  m_sync.add(to_add);
}

void mold::heating_valve_settings_cache::update(
    const mold::heating_valve_settings &to_update) {
  m_sync.update(to_update);
}

void mold::heating_valve_settings_cache::remove(
    const wolf::types::uuid_array &to_remove) {
  const std::string id_casted = wolf::base64::encode(to_remove);
  m_sync.remove(id_casted);
}

mold::heating_valve_settings_cache::settings_list
mold::heating_valve_settings_cache::get_all() {
  return m_sync.get_all();
}
