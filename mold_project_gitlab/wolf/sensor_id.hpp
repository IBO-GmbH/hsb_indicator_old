#ifndef WOLF_SENSOR_ID_HPP
#define WOLF_SENSOR_ID_HPP

#include "wolf/sensor_type.hpp"
#include "wolf/sensor_value_type.hpp"
#include "wolf/types.hpp"

namespace wolf {

// TODO refactor to sensor_profile? or sensor_information
class sensor_id {
 public:
  explicit sensor_id();
  explicit sensor_id(const types::id &id, const sensor_value_type type);
  explicit sensor_id(const types::id &id, const sensor_value_type type,
                     const types::id &owner);

  static sensor_id from_combined_id(const types::data &combined);
  static sensor_id from_combined_id_base64(const std::string &combined);
  types::data to_combined_id() const;
  std::string to_combined_id_base64() const;
  const types::id &get_id() const;
  const types::id &get_owner() const;
  sensor_value_type get_type() const;
  bool has_owner() const;
  bool operator==(const sensor_id &other) const;
  bool operator!=(const sensor_id &other) const;
  bool is_set() const;

 protected:
  types::id m_id;
  types::id m_owner;
  sensor_value_type m_type{sensor_value_type::unknown};
};

inline const types::id &sensor_id::get_id() const { return m_id; }
inline const types::id &sensor_id::get_owner() const { return m_owner; }
inline sensor_value_type sensor_id::get_type() const { return m_type; }

std::ostream &operator<<(std::ostream &os, const sensor_id &out);
std::ostream &operator<<(std::ostream &out,
                         const std::vector<sensor_id> &to_print);
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_HPP
