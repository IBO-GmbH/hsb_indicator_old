#ifndef MOLD_COMFORT_QUESTIONNAIRE_HPP
#define MOLD_COMFORT_QUESTIONNAIRE_HPP

#include <array>
#include <chrono>
#include <ostream>

namespace mold {

struct questionnaire_options {
  static constexpr int max_number_of_questionnaires = 15;
  bool enabled;
  int number_of_questionnaires;
  std::array<int, max_number_of_questionnaires> hours_time;
  std::array<int, max_number_of_questionnaires> minutes_time;
};

bool operator==(const questionnaire_options& first,
                const questionnaire_options& second);
bool operator!=(const questionnaire_options& first,
                const questionnaire_options& second);

std::ostream& operator<<(std::ostream& out,
                         const questionnaire_options to_print);

enum struct gender { female, male };

std::ostream& operator<<(std::ostream& out, const gender to_print);

enum struct age {
  twenty_to_thirty,
  thirty_one_to_forty,
  forty_one_to_fifty,
  fifty_one_to_sixty,
  older_than_sixty
};

std::ostream& operator<<(std::ostream& out, const age to_print);

enum struct weight {
  less_than_fifty,
  fifty_to_sixty_four,
  sixty_five_to_seventy_nine,
  eighty_to_hundred,
  more_than_hundred
};

std::ostream& operator<<(std::ostream& out, const weight to_print);

enum struct height {
  less_than_one_sixty,
  one_sixty_to_one_seventy,
  one_seventy_one_to_one_eighty,
  one_eighty_one_to_one_ninety,
  more_than_one_ninety
};

std::ostream& operator<<(std::ostream& out, const height to_print);

enum struct clothing {
  tropical,
  light_summer,
  light_work,
  indoors_winter,
  office_winter
};

std::ostream& operator<<(std::ostream& out, const clothing to_print);

enum struct activity {
  sitting_relaxed,
  sitting_activity,
  light_movement,
  heavier_movement,
  exhausting_activity
};

std::ostream& operator<<(std::ostream& out, const activity to_print);

enum struct temperature {
  chilly = -2,
  slightly_chilly = -1,
  exactly_right = 0,
  slightly_warm = 1,
  warm = 2
};

std::ostream& operator<<(std::ostream& out, const temperature to_print);

enum struct thermic_comfort {
  very_discomforting = -3,
  discomforting = -2,
  rather_discomforting = -1,
  rather_comforting = 1,
  comforting = 2,
  very_comforting = 3
};

std::ostream& operator<<(std::ostream& out, const thermic_comfort to_print);

enum struct heating {
  colder = -2,
  slightly_colder = -1,
  neutral = 0,
  slightly_warmer = 1,
  warmer = 2
};

std::ostream& operator<<(std::ostream& out, const heating to_print);

enum struct air_quality {
  stuffy = -2,
  slightly_stuffy = -1,
  neutral = 0,
  moderately_fresh = 1,
  pleasently_fresh = 2
};

std::ostream& operator<<(std::ostream& out, const air_quality to_print);

enum struct humidity {
  moist = -2,
  slightly_moist = -1,
  exactly_right = 0,
  slightly_dry = 1,
  dry = 2
};

std::ostream& operator<<(std::ostream& out, const humidity to_print);

struct questionnaire {
  using time_point = std::chrono::system_clock::time_point;
  time_point timestamp;
  gender gender_;
  age age_;
  weight weight_;
  height height_;
  clothing clothing_;
  activity activity_;
  temperature temperature_;
  thermic_comfort thermic_comfort_;
  heating heating_;
  air_quality air_quality_;
  humidity humidity_;
};

bool operator==(const questionnaire& first, const questionnaire& second);
bool operator!=(const questionnaire& first, const questionnaire& second);

std::ostream& operator<<(std::ostream& out, const questionnaire& to_print);

}  // namespace mold

#endif  // MOLD_COMFORT_QUESTIONNAIRE_HPP
