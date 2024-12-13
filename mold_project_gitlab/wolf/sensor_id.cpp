#include "sensor_id.hpp"
#include <experimental/iterator>
#include "base64.hpp"

using namespace wolf;

sensor_id::sensor_id() = default;

sensor_id::sensor_id(const types::id &id, const sensor_value_type type)
    : sensor_id(id, type, {}) {}

sensor_id::sensor_id(const types::id &id, const sensor_value_type value_type,
                     const types::id &owner)
    : m_id(id), m_owner(owner), m_type(value_type) {}

sensor_id sensor_id::from_combined_id_base64(const std::string &combined) {
  return from_combined_id(base64::decode(combined));
}

std::string sensor_id::to_combined_id_base64() const {
  return base64::encode(to_combined_id());
}

bool sensor_id::has_owner() const { return !m_owner.empty(); }

bool sensor_id::operator==(const sensor_id &other) const {
  return m_id == other.get_id() && m_type == other.get_type() &&
         m_owner == other.get_owner();
}

bool sensor_id::operator!=(const sensor_id &other) const {
  return !(other == *this);
}

bool sensor_id::is_set() const { return !m_id.empty(); }

types::data sensor_id::to_combined_id() const {
  types::data result;
  result.reserve(1 + m_id.size() + 1 + m_owner.size() + 1);
  auto add_size_and_id = [&result](const types::id &id) {
    result.push_back(static_cast<types::data::value_type>(id.size()));
    std::copy(id.cbegin(), id.cend(), std::back_inserter(result));
  };
  result.push_back(static_cast<std::uint8_t>(m_type));
  add_size_and_id(m_id);
  add_size_and_id(m_owner);

  return result;
}

sensor_id sensor_id::from_combined_id(const types::data &combined) {
  types::data::const_iterator current = combined.cbegin();
  auto read_id = [&current]() {
    types::id result;
    const std::size_t size{static_cast<std::size_t>(*current)};
    std::advance(current, 1);
    result.reserve(size);
    std::copy_n(current, size, std::back_inserter(result));
    std::advance(current, size);
    return result;
  };
  const auto type = static_cast<sensor_value_type>(combined.front());
  std::advance(current, 1);
  const types::id id = read_id();
  const types::id owner = read_id();
  return sensor_id(id, type, owner);
}

std::ostream &wolf::operator<<(std::ostream &os, const sensor_id &out) {
  return os << "{id:" << std::dec << out.get_id() << ", type:" << out.get_type()
            << ", owner:" << std::dec << out.get_owner() << "}";
}

std::ostream &wolf::operator<<(std::ostream &out,
                               const std::vector<sensor_id> &to_print) {
  out << "[";
  std::copy(to_print.cbegin(), to_print.cend(),
            std::experimental::make_ostream_joiner(out, ", "));
  return out << "]";
}
