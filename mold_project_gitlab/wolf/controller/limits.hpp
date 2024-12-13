#ifndef WOLF_CONTROLLER_LIMITS_HPP
#define WOLF_CONTROLLER_LIMITS_HPP

#include <chrono>
#include <ostream>

namespace wolf {

namespace controller {

struct limits {
  struct relative_humidity {
    float max = 60.f;
    float drH = 10.f;
  };

  struct temperature {
    float min = 18.f;
    float reference = 22.f;
  };

  struct air_quality {
    float co2 = 800.f;
    float co2_difference = 100.f;
    float sprout_value = 50.f;
    float sprout_value_difference = 10.f;
    float voc = 100.f;
    float voc_difference = 20.f;
  };

  relative_humidity humidity;
  float humidity_border = 70.f;
  temperature temperature_;
  air_quality air_quality_;
  float outdoor_temperature = 10.f;

  using wait_time = std::chrono::system_clock::duration;
  using minutes_in_float = std::chrono::duration<float, std::ratio<60>>;
  wait_time time = std::chrono::minutes(15);
};

bool operator==(const limits::relative_humidity& first,
                const limits::relative_humidity& second);
bool operator!=(const limits::relative_humidity& first,
                const limits::relative_humidity& second);

bool operator==(const limits::temperature& first,
                const limits::temperature& second);
bool operator!=(const limits::temperature& first,
                const limits::temperature& second);

bool operator==(const limits::air_quality& first,
                const limits::air_quality& second);
bool operator!=(const limits::air_quality& first,
                const limits::air_quality& second);

bool operator==(const limits& first, const limits& second);
bool operator!=(const limits& first, const limits& second);

std::ostream& operator<<(std::ostream& out, const limits& to_print);
}  // namespace controller
}  // namespace wolf
#endif  // WOLF_CONTROLLER_LIMITS_HPP
