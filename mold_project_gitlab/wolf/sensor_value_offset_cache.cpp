#include "sensor_value_offset_cache.hpp"

using namespace wolf;

namespace soci {
template <>
struct type_conversion<sensor_value_offset> {
  using base_type = values;
  static void from_base(values const &to_cast, indicator,
                        sensor_value_offset &result) {
    const auto id_encoded = to_cast.get<std::string>("sensor_id");
    result.id = sensor_id::from_combined_id_base64(id_encoded);
    result.offset = static_cast<float>(to_cast.get<double>("offset"));
  }
  static void to_base(const sensor_value_offset &to_cast, values &row,
                      indicator &indicator_) {
    row.set("sensor_id", to_cast.id.to_combined_id_base64());
    row.set("offset", static_cast<double>(to_cast.offset));
    indicator_ = i_ok;
  }
};

}  // namespace soci

sensor_value_offset_cache::sensor_value_offset_cache(database &database_)
    : m_logger{"sensor_value_offset_cache"},
      m_session(database_.get_session()) {
  create_tables();
}

void sensor_value_offset_cache::set(const sensor_value_offset &value) {
  m_session << "insert or replace into wolf_sensor_value_offset (sensor_id, "
               "offset) values (:sensor_id, :offset)",
      soci::use(value);
}

sensor_value_offset_cache::sensor_value_offsets
sensor_value_offset_cache::get_all() {
  sensor_value_offsets result;
  soci::rowset<sensor_value_offset> rows =
      m_session.prepare
      << "select sensor_id, offset from wolf_sensor_value_offset";
  std::copy(rows.begin(), rows.end(), std::back_inserter(result));
  return result;
}

void sensor_value_offset_cache::create_tables() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "create_tables";
  m_session << "create table if not exists wolf_sensor_value_offset "
               "(sensor_id text primary key, "
               "offset real not null)";
}
