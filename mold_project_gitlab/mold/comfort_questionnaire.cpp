#include "comfort_questionnaire.hpp"
#include <boost/assert.hpp>
#include <iomanip>
#include "wolf/timestamp_helper.hpp"

std::ostream &mold::operator<<(std::ostream &out, const mold::gender to_print) {
  switch (to_print) {
    case gender::female:
      return out << "female";
    case gender::male:
      return out << "male";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out, const mold::age to_print) {
  switch (to_print) {
    case age::twenty_to_thirty:
      return out << "20-30";
    case age::thirty_one_to_forty:
      return out << "31-40";
    case age::forty_one_to_fifty:
      return out << "41-50";
    case age::fifty_one_to_sixty:
      return out << "51-60";
    case age::older_than_sixty:
      return out << "older than 60";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out, const mold::weight to_print) {
  switch (to_print) {
    case weight::less_than_fifty:
      return out << "less than 50";
    case weight::fifty_to_sixty_four:
      return out << "50-64";
    case weight::sixty_five_to_seventy_nine:
      return out << "65-79";
    case weight::eighty_to_hundred:
      return out << "80-100";
    case weight::more_than_hundred:
      return out << "more than 100";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out, const mold::height to_print) {
  switch (to_print) {
    case height::less_than_one_sixty:
      return out << "less than 160";
    case height::one_sixty_to_one_seventy:
      return out << "160-170";
    case height::one_seventy_one_to_one_eighty:
      return out << "171-180";
    case height::one_eighty_one_to_one_ninety:
      return out << "181-190";
    case height::more_than_one_ninety:
      return out << "more than 190";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::activity to_print) {
  switch (to_print) {
    case activity::sitting_relaxed:
      return out << "relaxed sitting";
    case activity::sitting_activity:
      return out << "sitting activity";
    case activity::light_movement:
      return out << "light movement";
    case activity::heavier_movement:
      return out << "heavier movement";
    case activity::exhausting_activity:
      return out << "exhausting activity";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::clothing to_print) {
  switch (to_print) {
    case clothing::tropical:
      return out << "tropical clothing 0,3 clo";
    case clothing::light_summer:
      return out << "light summer clothing 0,5 clo";
    case clothing::light_work:
      return out << "light work clothing 0,8 clo";
    case clothing::indoors_winter:
      return out << "indoor winter clothing 1,0 clo";
    case clothing::office_winter:
      return out << "office clothing winter 1,2 clo";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::temperature to_print) {
  switch (to_print) {
    case temperature::chilly:
      return out << "chilly";
    case temperature::slightly_chilly:
      return out << "slightly chilly";
    case temperature::exactly_right:
      return out << "exactly right";
    case temperature::slightly_warm:
      return out << "slightly warm";
    case temperature::warm:
      return out << "warm";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::thermic_comfort to_print) {
  switch (to_print) {
    case thermic_comfort::very_discomforting:
      return out << "very discomforting";
    case thermic_comfort::discomforting:
      return out << "discomforting";
    case thermic_comfort::rather_discomforting:
      return out << "rather discomforting";
    case thermic_comfort::rather_comforting:
      return out << "rather comforting";
    case thermic_comfort::comforting:
      return out << "comforting";
    case thermic_comfort::very_comforting:
      return out << "very comforting";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::air_quality to_print) {
  switch (to_print) {
    case air_quality::stuffy:
      return out << "stuffy";
    case air_quality::slightly_stuffy:
      return out << "slightly stuffy";
    case air_quality::neutral:
      return out << "neutral";
    case air_quality::moderately_fresh:
      return out << "moderately fresh";
    case air_quality::pleasently_fresh:
      return out << "pleasently fresh";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::humidity to_print) {
  switch (to_print) {
    case humidity::moist:
      return out << "moist";
    case humidity::slightly_moist:
      return out << "slightly moist";
    case humidity::exactly_right:
      return out << "exactly right";
    case humidity::slightly_dry:
      return out << "slightly dry";
    case humidity::dry:
      return out << "dry";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::questionnaire &to_print) {
  wolf::timestamp_helper helper;
  return out << "{timestamp:" << std::dec
             << helper.cast_to_unix_seconds(to_print.timestamp)
             << ", gender:" << to_print.gender_
             << ", age in years:" << to_print.age_
             << ", weight in kg:" << to_print.weight_
             << ", height in cm:" << to_print.height_
             << ", clothing:" << to_print.clothing_
             << ", activity:" << to_print.activity_
             << ", temperature:" << to_print.temperature_
             << ", thermic_comfort:" << to_print.thermic_comfort_
             << ", thermic_change:" << to_print.heating_
             << ", air_quality:" << to_print.air_quality_
             << ", humidity:" << to_print.humidity_ << "}";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::questionnaire_options to_print) {
  out << "{enabled:" << to_print.enabled
      << ", number_of_questionnaires:" << to_print.number_of_questionnaires;
  for (unsigned int index = 0;
       index < questionnaire_options::max_number_of_questionnaires; ++index)
    out << ", time_" << (index + 1) << ": " << to_print.hours_time[index] << ":"
        << std::setfill('0') << std::setw(2) << to_print.minutes_time[index];
  out << "}";
  return out;
}

bool mold::operator==(const mold::questionnaire_options &first,
                      const mold::questionnaire_options &second) {
  return first.enabled == second.enabled &&
         first.number_of_questionnaires == second.number_of_questionnaires &&
         first.hours_time == second.hours_time &&
         first.minutes_time == second.minutes_time;
}

bool mold::operator!=(const mold::questionnaire_options &first,
                      const mold::questionnaire_options &second) {
  return !(first == second);
}

bool mold::operator==(const mold::questionnaire &first,
                      const mold::questionnaire &second) {
  return first.timestamp == second.timestamp &&
         first.gender_ == second.gender_ && first.age_ == second.age_ &&
         first.weight_ == second.weight_ && first.height_ == second.height_ &&
         first.clothing_ == second.clothing_ &&
         first.activity_ == second.activity_ &&
         first.temperature_ == second.temperature_ &&
         first.thermic_comfort_ == second.thermic_comfort_ &&
         first.air_quality_ == second.air_quality_ &&
         first.humidity_ == second.humidity_;
}

bool mold::operator!=(const mold::questionnaire &first,
                      const mold::questionnaire &second) {
  return !(first == second);
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::heating to_print) {
  switch (to_print) {
    case heating::colder:
      return out << "colder";
    case heating::slightly_colder:
      return out << "slightly_colder";
    case heating::neutral:
      return out << "neutral";
    case heating::slightly_warmer:
      return out << "slightly warmer";
    case heating::warmer:
      return out << "warmer";
  }
  BOOST_ASSERT_MSG(false, "must not be reached");
  return out << "unknown";
}
