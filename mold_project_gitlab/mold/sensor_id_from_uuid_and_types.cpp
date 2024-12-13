#include "sensor_id_from_uuid_and_types.hpp"
#include <boost/assert.hpp>
#include <boost/container_hash/hash.hpp>
#include "generate_sensor_id_from_config.hpp"

using namespace mold;

static wolf::types::id make_id(const wolf::types::uuid_array& uuid,
                               const wolf::sensor_type type) {
  wolf::types::id result;
  result.push_back(static_cast<std::uint8_t>(type));
  const auto sensor_id = generate_sensor_id_from_config::generate(uuid);
  std::copy(sensor_id.cbegin(), sensor_id.cend(), std::back_inserter(result));
  return result;
}

static wolf::types::id hash_id(const wolf::types::id& to_hash) {
  const std::size_t hashed =
      boost::hash_range(to_hash.cbegin(), to_hash.cend());
  const auto to_copy_from = reinterpret_cast<const std::uint8_t*>(&hashed);
  wolf::types::id result;
  std::copy_n(to_copy_from, sizeof(std::size_t), std::back_inserter(result));
  return result;
}

wolf::sensor_id sensor_id_from_uuid_and_types::create(
    const wolf::types::uuid_array& uuid, const wolf::sensor_type type,
    const wolf::sensor_value_type value_type) {
  BOOST_ASSERT_MSG(uuid != wolf::types::uuid_array{}, "invalid config_id");
  const auto sensor_id = make_id(uuid, type);
  const auto result_id = hash_id(sensor_id);
  return wolf::sensor_id{result_id, value_type};
}
