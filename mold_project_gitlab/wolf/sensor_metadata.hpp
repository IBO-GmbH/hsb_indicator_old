#ifndef WOLF_SENSOR_METADATA_HPP
#define WOLF_SENSOR_METADATA_HPP

#include <map>
#include <string>
#include <vector>

namespace wolf {

struct sensor_metadata {
 public:
  std::string type;
  float min_value;
  float max_value;
  std::map<std::string, std::string> properties;

  bool operator==(const sensor_metadata& other) const;
  bool operator!=(const sensor_metadata& other) const;
};

std::ostream& operator<<(std::ostream& os, const sensor_metadata& out);
}  // namespace wolf

#endif  // WOLF_SENSOR_METADATA_HPP
