#include "program_options_parser.hpp"
#include "mold/version.hpp"

using namespace mold;

const config &program_options_parser::get_config_mold() { return m_config; }

program_options_parser::additional_options
program_options_parser::create_additional_options() {
  program_options_parser::additional_options result =
      base::create_additional_options();
  {
    using namespace boost::program_options;
    options_description general("Mold General");
    general.add_options()("allowed-eep",
                          value<std::string>(&m_allowed_eep)
                              ->default_value("0xa50401")
                              ->required(),
                          "set the the only allowed eep as hex number")(
        "mold-stylesheet",
        value<std::string>(&m_config.general_.stylesheet)
            ->default_value("stylesheet.qss")
            ->required(),
        "Set the stylesheet file.")(
        "mold-language",
        value<std::string>(&m_config.general_.language)
            ->default_value("en")
            ->required(),
        "Set default language")(
        "mold-language-de",
        value<std::string>(&m_config.general_.language_de)
            ->default_value("build/mold/mold_de.qm")
            ->required(),
        "Path to german language file")(
        "mold-use-sensor-offset",
        value<bool>(&m_config.general_.use_sensor_offset)
            ->default_value(false)
            ->required(),
        "Enables ui to offer sensor offset.")(
        "mold-confirm-beep-interval",
        value<int>(&m_config.general_.confirm_beep_interval)
            ->default_value(4)
            ->required(),
        "Sets the beep interval till confirmation in seconds.")(
        "mold-resync-timeout",
        value<int>(&m_config.general_.resync_timeout)
            ->default_value(600)
            ->required(),
        "Sets timer interval, in which the a resync shall get triggered.")(
        "mold-energy-save-interval",
        value<int>(&m_config.general_.energy_save_interval)
            ->default_value(60)
            ->required(),
        "Sets the interval, in which the accumulated power consumption is "
        "saved to the database in minutes.");
    options_description login("Login");
    login.add_options()("secret-file",
                        value<std::string>(&m_config.login_.secret_file)
                            ->default_value("/root/secret"),
                        "path to file, containing secret for login");
    login.add_options()(
        "login-version",
        value<std::string>(&m_config.login_.version)
            ->default_value(mold::version::get_string()),
        "login version, can be used for version update testing");
    options_description initial("Mold Initial");
    initial.add_options()("initial-outdoor-sensor",
                          value<std::string>(&m_initial_outdoor_sensor_read)
                              ->default_value("0x00000000")
                              ->required(),
                          "set the initial outdoor sensor id as hex number")(
        "initial-room-name",
        value<std::string>(&m_config.initial_.room_name)
            ->default_value("Smart Box")
            ->required(),
        "set the initial room name as utf8 string")(
        "initial-room-id",
        value<std::string>(&m_config.initial_.room_id)
            ->default_value("01020304-0102-0102-0102-010203040506")
            ->required(),
        "set the id of the initial room as utf8 string");
    options_description building_level("Mold Building Level");
    building_level.add_options()(
        "mold-level-new-building",
        value<float>(&m_config.building_level_.new_)
            ->default_value(0.71f)
            ->required(),
        "set the level of the mold calculation when it should turn yellow, for "
        "\"new buildings\".")("mold-level-old-building",
                              value<float>(&m_config.building_level_.old)
                                  ->default_value(0.67f)
                                  ->required(),
                              "set the level of the mold calculation when it "
                              "should turn yellow, for \"old buildings\".");
    options_description display("Mold Display");
    display.add_options()(
        "mold-display-timeout",
        value<int>(&m_config.display_.timeout)->default_value(180)->required(),
        "Timeout in seconds when display should shut off. Set to <=0 to "
        "disable.")("mold-display-on-script",
                    value<std::string>(&m_config.display_.on_script)
                        ->default_value("./scripts/display_on")
                        ->required(),
                    "The script to call to shut the display on.")(
        "mold-display-off-script",
        value<std::string>(&m_config.display_.off_script)
            ->default_value("./scripts/display_off")
            ->required(),
        "The script to call to shut the display off.");
    options_description room("Mold Room");
    room.add_options()(
        "mold-switch-room",
        value<int>(&m_config.room_.switch_timeout)
            ->default_value(10)
            ->required(),
        "Sets the timeout in seconds after a room should switch.")(
        "mold-max-room-count",
        value<int>(&m_config.room_.max_count)->default_value(0)->required(),
        "Sets the maximal count of rooms.")(
        "mold-max-room-count-per-page",
        value<int>(&m_config.room_.max_count_per_page)
            ->default_value(8)
            ->required(),
        "Sets the maximal count of rooms shown per page.");
    options_description states("Mold States");
    states.add_options()(
        "mold-green-timeout",
        value<int>(&m_config.mold_states_.green_timeout)
            ->default_value(0)
            ->required(),
        "Timeout in seconds when mold state shall change from NOT green to "
        "green.")("mold-not-red-message-timeout",
                  value<int>(&m_config.mold_states_.not_red_message_timeout)
                      ->default_value(3600)
                      ->required(),
                  "Timeout in seconds for message \"keep ventialting\" after "
                  "state red->green.")(
        "mold-calculation-timeout",
        value<int>(&m_config.mold_states_.calculation_timeout)
            ->default_value(180)
            ->required(),
        "Timeout in minutes a value is allowed "
        "to be old to calculate mold-value.")(
        "mold-yellow-threshold",
        value<float>(&m_config.mold_states_.yellow_threshold)
            ->default_value(.05f)
            ->required(),
        "Threshold when mold traffic light changes to yellow as decimal.")(
        "mold-red-threshold",
        value<float>(&m_config.mold_states_.red_threshold)
            ->default_value(.5f)
            ->required(),
        "Threshold when mold traffic light changes to red as decimal.");
    options_description gsm("Mold GSM");
    gsm.add_options()("mold-gsm-info-timeout",
                      value<int>(&m_config.gsm_.info_read_timeout)
                          ->default_value(10)
                          ->required(),
                      "Sets the interval when to read iccid, rssi and provider "
                      "from 3g stick.");
    options_description gui("Mold Graphic User Interface");
    gui.add_options()(
        "use-gui",
        value<bool>(&m_config.use_gui)->default_value(false)->required(),
        "use display gui")("gui-style",
                           value<std::string>(&m_config.gui_.style)
                               ->default_value("Fusion")
                               ->required(),
                           "style");
    options_description nightmode("Nightmode");
    nightmode.add_options()("hours-on",
                            value<int>(&m_config.nightmode_.on.hours)
                                ->default_value(22)
                                ->required(),
                            "hours of activation time")(
        "minutes-on",
        value<int>(&m_config.nightmode_.on.minutes)
            ->default_value(0)
            ->required(),
        "minutes of activation time")("hours-off",
                                      value<int>(&m_config.nightmode_.off.hours)
                                          ->default_value(6)
                                          ->required(),
                                      "hours of deactivation time")(
        "minutes-off",
        value<int>(&m_config.nightmode_.off.minutes)
            ->default_value(0)
            ->required(),
        "minutes of deactivation time")(
        "check-interval",
        value<int>(&m_config.nightmode_.check_interval)
            ->default_value(30)
            ->required(),
        "interval to check nightmode in minutes");
    options_description median("Median");
    median.add_options()(
        "use-median",
        value<bool>(&m_config.median_.use_median)
            ->default_value(true)
            ->required(),
        "if median should be used for calculating evaluation humidity")(
        "evaluation-humidity-median-time",
        value<int>(&m_config.median_.evaluation_humidity_median_time)
            ->default_value(60)
            ->required(),
        "how many minutes evaluation humidity should be taken for median");
    options_description evaluation_humidity("Evaluation humidity");
    evaluation_humidity.add_options()(
        "evaluation-humidity-filter-constant",
        value<float>(&m_config.evaluation_humidity_.filter_constant)
            ->default_value(0.25f)
            ->required(),
        "minimum change for evaluation humidity mean value for getting "
        "processed. Set to one to disable.")(
        "evaluation-humidity-filter-threshold",
        value<float>(&m_config.evaluation_humidity_.filter_threshold)
            ->default_value(2.0f)
            ->required(),
        "minimum change for evaluation humidity mean value for getting "
        "processed. Set to zero to disable.")(
        "evaluation-humidity-filter-threshold-with-time",
        value<float>(&m_config.evaluation_humidity_.filter_threshold_with_time)
            ->default_value(2.0f * 600.f)
            ->required(),
        "minimum change with time for evaluation humidity mean value for "
        "getting processed.")(
        "evaluation-humidity-filter-timeout",
        value<int>(&m_config.evaluation_humidity_.filter_threshold_timeout)
            ->default_value(15 * 60)
            ->required(),
        "timeout in seconds when to ignore epsilon check.");
    options_description mold_value("Mold Value");
    mold_value.add_options()(
        "mold-value-filter-constant",
        value<float>(&m_config.mold_value_.filter_constant)
            ->default_value(0.5f)
            ->required(),
        "minimum change for mold value mean value for getting "
        "processed. Set to one to disable.")(
        "mold-value-filter-threshold",
        value<float>(&m_config.mold_value_.filter_threshold)
            ->default_value(.005f)
            ->required(),
        "minimum change for mold value mean value for getting "
        "processed. Set to zero to disable.")(
        "mold-value-filter-threshold-with-time",
        value<float>(&m_config.mold_value_.filter_threshold_with_time)
            ->default_value(.005f * 600.f)
            ->required(),
        "minimum change with time for mold value mean value for "
        "getting processed.")(
        "mold-value-filter-timeout",
        value<int>(&m_config.mold_value_.filter_threshold_timeout)
            ->default_value(15 * 60)
            ->required(),
        "timeout in seconds when to ignore epsilon check.");
    options_description mold_options;
    mold_options.add(general)
        .add(login)
        .add(initial)
        .add(building_level)
        .add(display)
        .add(room)
        .add(states)
        .add(gsm)
        .add(gui)
        .add(nightmode)
        .add(median)
        .add(evaluation_humidity)
        .add(mold_value);
    result.push_back(std::move(mold_options));
  }
  return result;
}

void program_options_parser::parse_additional_options(
    const boost::program_options::variables_map & /*vm*/) {
  {
    std::istringstream interpreter(m_initial_outdoor_sensor_read);
    interpreter >> std::hex >> m_config.initial_.outdoor_sensor;
  }
  {
    std::istringstream interpreter(m_allowed_eep);
    std::uint32_t to_convert;
    interpreter >> std::hex >> to_convert;
    m_config.general_.eep[0] = (to_convert >> 16) & 0xff;
    m_config.general_.eep[1] = (to_convert >> 8) & 0xff;
    m_config.general_.eep[2] = (to_convert >> 0) & 0xff;
  }
}
