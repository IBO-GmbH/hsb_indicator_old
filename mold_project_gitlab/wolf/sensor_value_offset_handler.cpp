#include "sensor_value_offset_handler.hpp"

#include "wolf/math_compare_decimals.hpp"

using namespace wolf;

struct same_id {
  const sensor_id& check;
  bool operator()(const sensor_value_offset other) { return check == other.id; }
};

sensor_value_offset_handler::sensor_value_offset_handler(
    sensor_value_offset_cache& database)
    : m_logger{"sensor_value_offset_handler"}, m_database(database) {
  load_from_cache();
}

void sensor_value_offset_handler::set(const sensor_value_offset offset) {
  auto found =
      std::find_if(m_offsets.begin(), m_offsets.end(), same_id{offset.id});
  sensor_value_offset old{offset.id, 0.f};
  if (found == m_offsets.cend())
    m_offsets.push_back(offset);
  else {
    if (math::compare_floats(offset.offset, found->offset)) return;
    old = *found;
    *found = offset;
  }
  m_database.set(offset);
  signal_set(old, offset);
}

std::optional<float> sensor_value_offset_handler::get(
    const sensor_id& id) const {
  auto found = std::find_if(m_offsets.cbegin(), m_offsets.cend(), same_id{id});
  if (found == m_offsets.cend()) return {};
  return found->offset;
}

float sensor_value_offset_handler::apply_offset_to_value(
    const sensor_id& id, const float value) const {
  const auto offset = get(id);
  return value + offset.value_or(0.f);
}

void sensor_value_offset_handler::load_from_cache() {
  m_offsets = m_database.get_all();
}
