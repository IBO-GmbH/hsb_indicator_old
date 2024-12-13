#ifndef MOLD_PROGRAM_OPTIONS_PARSER_HPP
#define MOLD_PROGRAM_OPTIONS_PARSER_HPP

#include "wolf/program_options_parser.hpp"

namespace mold {

struct config {
  struct general {
    wolf::types::eep eep;
    std::string stylesheet;
    std::string language;
    std::string language_de;
    bool use_sensor_offset;
    int confirm_beep_interval;
    int resync_timeout;
    int energy_save_interval;
  };
  general general_;

  struct login {
    std::string secret_file;
    std::string version;
  };
  login login_;

  struct initial {
    std::uint32_t outdoor_sensor;
    std::string room_name;
    std::string room_id;
  };
  initial initial_;

  struct building_level {
    float old;
    float new_;
  };
  building_level building_level_;

  struct display {
    int timeout;
    std::string on_script;
    std::string off_script;
  };
  display display_;

  struct room {
    int switch_timeout;
    int max_count;
    int max_count_per_page;
  };
  room room_;

  struct mold_states {
    int green_timeout;
    int not_red_message_timeout;
    int calculation_timeout;  // in minutes
    float yellow_threshold;
    float red_threshold;
  };
  mold_states mold_states_;

  struct gsm {
    int info_read_timeout;  // in seconds
  };
  gsm gsm_;

  bool use_gui;
  struct gui {
    std::string style;
  };
  gui gui_;

  struct time {
    int hours;
    int minutes;
  };

  struct nightmode {
    time on;
    time off;
    int check_interval;
  };
  nightmode nightmode_;

  struct median {
    bool use_median;
    int evaluation_humidity_median_time;
  };
  median median_;

  struct evaluation_humidity {
    float filter_constant;
    float filter_threshold;
    float filter_threshold_with_time;
    int filter_threshold_timeout;
  };
  evaluation_humidity evaluation_humidity_;

  struct mold_value {
    float filter_constant;
    float filter_threshold;
    float filter_threshold_with_time;
    int filter_threshold_timeout;
  };
  mold_value mold_value_;
};

class program_options_parser : public wolf::program_options_parser {
 public:
  using base = wolf::program_options_parser;

  program_options_parser() = default;

  const config& get_config_mold();

 protected:
  additional_options create_additional_options() override;
  void parse_additional_options(
      const boost::program_options::variables_map& vm) override;

 private:
  config m_config;
  std::string m_initial_outdoor_sensor_read;
  std::string m_allowed_eep;
};
}  // namespace mold

#endif  // MOLD_PROGRAM_OPTIONS_PARSER_HPP
